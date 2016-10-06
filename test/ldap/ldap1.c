#include <stdio.h>
#include <ldap.h>

#define LDAP_SERVER "ldap://ldap.forumsys.com:389"

int main (int argc, char **argv) 
{
    LDAP        *ld;
    int         rc;
    char        bind_dn[100];
    LDAPMessage *result, *e;
    char *dn;
    int has_value;

    sprintf (bind_dn, "uid=boyle,dc=example,dc=com");
    printf( "Connecting as %s\n", bind_dn );

    if (ldap_initialize (&ld, LDAP_SERVER)) {
        perror( "ldap_initialize" );
        return 1;
    }

    rc = ldap_simple_bind_s (ld, bind_dn, "password");
    if (rc != LDAP_SUCCESS) {
        fprintf (stderr, "ldap_simple_bind_s: %s\n", ldap_err2string (rc));
        return 1;
    }

    printf( "Successful authentication\n" );

    rc = ldap_search_ext_s (ld, "dc=ashwin,dc=com", LDAP_SCOPE_SUBTREE, "sn=ashwin kumar", NULL, 0, NULL, NULL, NULL, 0, &result);
    if (rc != LDAP_SUCCESS) {
        fprintf (stderr, "ldap_search_ext_s: %s\n", ldap_err2string(rc));
    }

    for (e = ldap_first_entry (ld, result); e != NULL; e = ldap_next_entry (ld, e)) {
        if ((dn = ldap_get_dn (ld, e)) != NULL) {
            printf ("dn: %s\n", dn);
            has_value = ldap_compare_s (ld, dn, "userPassword", "secret"); 
            switch (has_value) { 
                case LDAP_COMPARE_TRUE: 
                    printf ("Works.\n"); 
                    break; 
                case LDAP_COMPARE_FALSE: 
                    printf ("Failed.\n"); 
                    break; 
                default: 
                    ldap_perror (ld, "ldap_compare_s"); 
                    return 1; 
            } 
            ldap_memfree (dn);
        }
    }

    ldap_msgfree (result);
    ldap_unbind (ld);
    return 0;
}
