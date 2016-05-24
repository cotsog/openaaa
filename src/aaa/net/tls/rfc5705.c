


/*

If no context is provided, it then computes:

           PRF(SecurityParameters.master_secret, label,
               SecurityParameters.client_random +
               SecurityParameters.server_random
               )[length]

If context is provided, it computes:

           PRF(SecurityParameters.master_secret, label,
               SecurityParameters.client_random +
               SecurityParameters.server_random +
               context_value_length + context_value
               )[length]

*/
