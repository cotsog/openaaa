%module(package="com.opensec.aaa", jniclassname="Link", docstring="Bindings for AAA") "Attr"

/*%include "enums.swg"*/
/* Force the generated Java code to use the C constant values rather than making a JNI call */
%javaconst(1);

%pragma(java) jniclasscode=%{
static {
        try {
                System.loadLibrary("aaa");
        } catch (UnsatisfiedLinkError e) {
                System.err.println("Native code library failed to load. \n" + e);
                System.exit(1);
        }
}
%}

%include <typemaps.i>
%include <stdint.i>
%include <carrays.i>
%include <cpointer.i>

/*
%typemap(javabase) AAATYPE, AAATYPE *, AAATYPE &, AAATYPE [],
AAATYPE (CLASS::*) "SWIG"

%typemap(javacode) AAATYPE, AAATYPE *, AAATYPE &, AAATYPE [],
AAATYPE (CLASS::*) %{
  protected long getPointer() {
    return swigCPtr;
  }
%}
*/

/*
%rename("%(strip:[AAA_])s") "";
%rename("%(strip:[AAA_ATTR_])s") "";
%rename("%(strip:[AAA_BIND_])s") "";
*/

%rename(Bind) aaa_bind_e;
%rename(Option) aaa_opt_e;
%rename(Client) aaa;


%{
#include <aaa/lib.h>
%}

%rename("%(strip:[AAA_])s") "";

%include <aaa/lib.h>

struct Client {
        %extend {
                Client() {
                        struct aaa *aaa = NULL;
                        aaa = aaa_new();
                        return (struct Client *)aaa;
                }
                ~Client() { 
                        aaa_free((struct aaa*)$self);
                }

                int bind(enum aaa_bind_e k, const char *id) {
                        return aaa_bind((struct aaa*)$self, k, id);
                }

                int bind(enum aaa_bind_e k) {
                        return aaa_bind((struct aaa*)$self, k, NULL);
                }

                int setAttribute(const char *attr, char *value) {
                        return aaa_attr_set((struct aaa*)$self, attr, value);
                }

                const char *getAttribute(const char *attr) {
                        return aaa_attr_get((struct aaa*)$self, attr);
                }

                int select(const char *path) {
                        return aaa_select((struct aaa*)$self, path);
                }

                int select() {
                        return aaa_select((struct aaa*)$self, NULL);
                }

                int commit() {
                        return aaa_commit((struct aaa*)$self);
                }
        }
};
