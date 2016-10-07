/* --------------------------------------------------------------------------- *
 * file:        ad_ldaptest.c v1.0                                             *
 * author:      10/19/2010 Frank4DD, see http://fm4dd.com/programming          *
 * purpose:     This test program connects to a Actice Directory LDAP using    *
 *              simple bind and returns the groups for a given domain user.    *
 *                                                                             *
 * compile:     gcc -o ad_ldaptest ad_ldaptest.c -lldap                        *
 * --------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>

#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wc++11-compat-deprecated-writable-strings"

int main( int argc, char **argv ) {

  LDAP *ldap;
  LDAPMessage *answer, *entry;
  BerElement *ber;

  int  result;
  int  auth_method    = LDAP_AUTH_SIMPLE;
  int  ldap_version   = LDAP_VERSION3;
  char *ldap_host     = "ldap.forumsys.com";
  int   ldap_port     = 389;
  char *ldap_dn       = "uid=boyle,dc=example,dc=com";
  char *ldap_pw       = "password";
  char *base_dn       = "dc=example,dc=com";

  // The search scope must be either LDAP_SCOPE_SUBTREE or LDAP_SCOPE_ONELEVEL
  int  scope          = LDAP_SCOPE_SUBTREE;
  // The search filter, "(objectClass=*)" returns everything. Windows can return
  // 1000 objects in one search. Otherwise, "Size limit exceeded" is returned.
  char *filter        = "(objectClass=*)";
  // The attribute list to be returned, use {NULL} for getting all attributes
  char *attrs[]       = {NULL};
  // Specify if only attribute types (1) or both type and value (0) are returned
  int  attrsonly      = 0;
  // entries_found holds the number of objects found for the LDAP search
  int  entries_found  = 0;
  // dn holds the DN name string of the object(s) returned by the search
  char *dn            = "";
  // attribute holds the name of the object(s) attributes returned
  char *attribute     = "";
  // values is  array to hold the attribute values of the object(s) attributes
  char **values;
  // i is the for loop variable to cycle through the values[i]
  int  i              = 0;

  /* First, we print out an informational message. */
  printf( "Connecting to host %s at port %d...\n\n", ldap_host, ldap_port );

  /* STEP 1: Get a LDAP connection handle and set any session preferences. */
  /* For ldaps we must call ldap_sslinit(char *host, int port, int secure) */
  if ( (ldap = ldap_init(ldap_host, ldap_port)) == NULL ) {
    perror( "ldap_init failed" );
    exit( EXIT_FAILURE );
  } else {
    printf("Generated LDAP handle.\n");
  }

  /* The LDAP_OPT_PROTOCOL_VERSION session preference specifies the client */
  /* is an LDAPv3 client. */
  result = ldap_set_option(ldap, LDAP_OPT_PROTOCOL_VERSION, &ldap_version);

  if ( result != LDAP_OPT_SUCCESS ) {
      ldap_perror(ldap, "ldap_set_option failed!");
      exit(EXIT_FAILURE);
  } else {
    printf("Set LDAPv3 client version.\n");
  }

  /* STEP 2: Bind to the server. */
  // If no DN or credentials are specified, we bind anonymously to the server */
  // result = ldap_simple_bind_s( ldap, NULL, NULL );
  result = ldap_simple_bind_s(ldap, ldap_dn, ldap_pw );

  if ( result != LDAP_SUCCESS ) {
    fprintf(stderr, "ldap_simple_bind_s: %s\n", ldap_err2string(result));
    exit(EXIT_FAILURE);
  } else {
    printf("LDAP connection successful.\n");
  }

  /* STEP 3: Do the LDAP search. */
  result = ldap_search_s(ldap, base_dn, scope, filter,
                         attrs, attrsonly, &answer);

  if ( result != LDAP_SUCCESS ) {
    fprintf(stderr, "ldap_search_s: %s\n", ldap_err2string(result));
    exit(EXIT_FAILURE);
  } else {
    printf("LDAP search successful.\n");
  }

  /* Return the number of objects found during the search */
  entries_found = ldap_count_entries(ldap, answer);
  if ( entries_found == 0 ) {
    fprintf(stderr, "LDAP search did not return any data.\n");
    exit(EXIT_FAILURE);
  } else {
    printf("LDAP search returned %d objects.\n", entries_found);
  }

  /* cycle through all objects returned with our search */
  for ( entry = ldap_first_entry(ldap, answer);
        entry != NULL;
        entry = ldap_next_entry(ldap, entry)) {

    /* Print the DN string of the object */
    dn = ldap_get_dn(ldap, entry);
    printf("Found Object: %s\n", dn);

    // cycle through all returned attributes
    for ( attribute = ldap_first_attribute(ldap, entry, &ber);
          attribute != NULL;
          attribute = ldap_next_attribute(ldap, entry, ber)) {

      /* Print the attribute name */
      printf("Found Attribute: %s\n", attribute);
      if ((values = ldap_get_values(ldap, entry, attribute)) != NULL) {

        /* cycle through all values returned for this attribute */
        for (i = 0; values[i] != NULL; i++) {

          /* print each value of a attribute here */
          printf("%s: %s\n", attribute, values[i] );
        }
        ldap_value_free(values);
      }
    }
    ldap_memfree(dn);
  }

  ldap_msgfree(answer);
  ldap_unbind(ldap);
  return(EXIT_SUCCESS);
}
