/*
Copyright (©) 2003-2016 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <filter/ldap.h>
#include <ldap.h>


#pragma clang diagnostic ignored "-Wdeprecated-declarations" // Todo out?


bool filter_ldap_get (string uri, string user, string password, bool & access, string & email) // Todo
{
  LDAP *ldap;
  LDAPMessage *answer, *entry;
  BerElement *ber;
  
  int result;
  int auth_method = LDAP_AUTH_SIMPLE;
  int ldap_version = LDAP_VERSION3;
  const char *ldap_uri = "ldap://ldap.forumsys.com:389";
  const char *ldap_dn = "uid=boyle,dc=example,dc=com";
  const char *ldap_pw = "password";
  const char *base_dn = "dc=example,dc=com";
  
  // The search scope must be either LDAP_SCOPE_SUBTREE or LDAP_SCOPE_ONELEVEL
  int scope = LDAP_SCOPE_SUBTREE;
  // The search filter, "(objectClass=*)" returns everything. Windows can return
  // 1000 objects in one search. Otherwise, "Size limit exceeded" is returned.
  const char *filter = "(objectClass=*)";
  // The attribute list to be returned, use {NULL} for getting all attributes
  char *attrs[] = {NULL};
  // Specify if only attribute types (1) or both type and value (0) are returned
  int attrsonly = 0;
  // entries_found holds the number of objects found for the LDAP search
  int entries_found = 0;
  // dn holds the DN name string of the object(s) returned by the search
  char *dn = (char *) "";
  // attribute holds the name of the object(s) attributes returned
  const char *attribute = "";
  // values is  array to hold the attribute values of the object(s) attributes
  char **values;
  // i is the for loop variable to cycle through the values[i]
  int i = 0;
  
  cout << "Connecting to " << ldap_uri << endl; // Todo
  
  // Get a LDAP connection handle for the URI.
  result = ldap_initialize (&ldap, ldap_uri);
  if (result != LDAP_SUCCESS) {
    cerr << ldap_err2string (result) << endl; // Todo
    return false;
  } else {
    printf("Generated LDAP handle.\n");
  }
  
  // Specify to use the LDAPv3 protocol.
  result = ldap_set_option (ldap, LDAP_OPT_PROTOCOL_VERSION, &ldap_version);
  if (result != LDAP_OPT_SUCCESS ) {
    cerr << ldap_err2string (result) << endl; // Todo
  } else {
    printf("Set LDAPv3 protocol.\n");
  }
  
#ifdef TODO
  /* STEP 2: Bind to the server. */
  // If no DN or credentials are specified, we bind anonymously to the server */
  // result = ldap_simple_bind_s( ldap, NULL, NULL );
  result = ldap_simple_bind_s (ldap, ldap_dn, ldap_pw );
  if (result != LDAP_SUCCESS) {
    fprintf(stderr, "ldap_simple_bind_s: %s\n", ldap_err2string(result));
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
  ldap_unbind_ext (ldap, NULL, NULL);
#endif
  
  (void) uri;
  (void) user;
  (void) password;
  (void) access;
  (void) email;
  
  // Done.
  return true;
}
