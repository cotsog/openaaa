package com.opensec.aaa;

public class Test {

    public static void sessionCreate(Client aaa, String sid) {

        aaa.bind(Bind.BIND_SESSION_ID, sid);

        aaa.setAttribute(Attr.ATTR_USER_NAME, "nobody");
        aaa.setAttribute(Attr.ATTR_USER_ID,"1");
        aaa.setAttribute(Attr.ATTR_USER_EMAIL,"text@example.com");
        aaa.setAttribute(Attr.ATTR_SESSION_EXPIRES, "300");

        aaa.setAttribute("user.groups[]", "test");
        aaa.setAttribute("user.groups.test.roles[]", "editor");

        aaa.commit();

        System.out.println("sess::create(sid=" + sid + ")");
    }

    public static void sessionAttach(Client aaa, String sid) {

        aaa.bind(Bind.BIND_SESSION_ID, sid);
        aaa.select();

    }

    public static void main(String argv[]) {
        com.opensec.aaa.Client aaa = new com.opensec.aaa.Client();

        sessionCreate(aaa, "9F4D4F91FF8569B3B2860CC6B8BB1AB27325D1");
        sessionAttach(aaa, "9F4D4F91FF8569B3B2860CC6B8BB1AB27325D1");

        System.out.println("OK");
    }
}
