/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/


#include <stdio.h>
#include "php.h"
#include <unistd.h>
#include <signal.h>
#include <locale.h>
#include "zend.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_main.h"
#include "ext/standard/php_standard.h"
#include "php_getopt.h"
#include "php_cli_server.h"

#define php_select(m, r, w, e, t)	select(m, r, w, e, t)

const opt_struct OPTIONS[] = {
	{'-', 0, NULL}
};


// Overwriteable ini defaults must be set in sapi_cli_ini_defaults().
#define INI_DEFAULT(name,value)\
	Z_SET_REFCOUNT(tmp, 0);\
	Z_UNSET_ISREF(tmp);	\
	ZVAL_STRINGL(&tmp, zend_strndup(value, sizeof(value)-1), sizeof(value)-1, 0);\
	zend_hash_update(configuration_hash, name, sizeof(name), &tmp, sizeof(zval), NULL);\

static void sapi_cli_ini_defaults(HashTable *configuration_hash)
{
	zval tmp;
	INI_DEFAULT("report_zend_debug", "0");
	INI_DEFAULT("display_errors", "1");
}


ZEND_BEGIN_ARG_INFO(arginfo_dl, 0)
	ZEND_ARG_INFO(0, extension_filename)
ZEND_END_ARG_INFO()


int server(int argc, char *argv[], char * document_root)
{
#ifdef ZTS
	void ***tsrm_ls;
#endif
	int exit_status = SUCCESS;
	int module_started = 0, sapi_started = 0;
	char *ini_path_override = NULL;
	char *ini_entries = NULL;
	int ini_ignore = 0;
	sapi_module_struct * sapi_module = NULL;

	// Ignore SIGPIPE in standalone mode so that sockets created via fsockopen()don't kill PHP if the remote site	closes it.
  signal(SIGPIPE, SIG_IGN);

#ifdef ZTS
	tsrm_startup(1, 1, 0, NULL);
	tsrm_ls = ts_resource(0);
#endif

	sapi_module = &cli_server_sapi_module;

	sapi_module->ini_defaults = sapi_cli_ini_defaults;
	sapi_module->php_ini_path_override = ini_path_override;
	sapi_module->phpinfo_as_text = 1;
	sapi_module->php_ini_ignore_cwd = 1;
	sapi_startup(sapi_module);
	sapi_started = 1;

	sapi_module->php_ini_ignore = ini_ignore;

	sapi_module->executable_location = argv[0];

	sapi_module->ini_entries = ini_entries;

	/* startup after we get the above ini override se we get things right */
	if (sapi_module->startup(sapi_module) == FAILURE) {
		/* there is no way to see if we must call zend_ini_deactivate()
		 * since we cannot check if EG(ini_directives) has been initialised
		 * because the executor's constructor does not set initialize it.
		 * Apart from that there seems no need for zend_ini_deactivate() yet.
		 * So we goto out_err.*/
		exit_status = 1;
		goto out;
	}
	module_started = 1;

	zend_first_try {
		exit_status = do_cli_server(argc, argv, document_root TSRMLS_CC);
	} 
  zend_end_try();
out:
	if (ini_entries) {
		free(ini_entries);
	}
	if (module_started) {
		php_module_shutdown(TSRMLS_C);
	}
	if (sapi_started) {
		sapi_shutdown();
	}
#ifdef ZTS
	tsrm_shutdown();
#endif

	exit(exit_status);
}


