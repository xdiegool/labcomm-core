import org.stringtemplate.v4.ST;
import org.stringtemplate.v4.STGroup;
import org.stringtemplate.v4.STGroupDir;
import org.stringtemplate.v4.STGroupFile;

import java.util.Arrays;
import java.util.ArrayList;

public class StringTemplateTest {


    public static void test1() {
        ST st = new ST("Hello, $object$!", '$', '$');
        st.add("object", "World");
        System.out.println(st.render());
    }

    public static void test2() {
        STGroup group = new STGroupDir(".", '$', '$');
        ST st = group.getInstanceOf("test2");
        st.add("object", "World");
        st.add("attr", "cruel");
        System.out.println(st.render());
    }

    public static void test3() {
        STGroup group = new STGroupFile("test3.stg", '$', '$');
        ST st = group.getInstanceOf("test3a");
        st.add("object", "cat");
        st.add("attr", "fluffy");
        System.out.println(st.render());
        ST st2 = group.getInstanceOf("test3b");
        st2.add("object", "cat");
        System.out.println(st2.render());
    }

    public static void test4() {
        ST st = new ST("A list: $content; separator=\";\\n        \"$;", '$', '$');
        st.add("content", "Kalle");
        st.add("content", "Nisse");
        st.add("content", "Pelle");
        System.out.println(st.render());
    }

    public static void test5() {
        ST st = new ST("Another list: $content; separator=\";\\n              \"$;", '$', '$');
        st.add("content", new String[]{"Hej", "Du", "Glade", "Programmerare"});
        System.out.println(st.render());
    }

    public static void test6() {
        ST st = new ST("A list:\n $content: {x | My favourite colour is $x$...\n}$done!", '$', '$');
        st.add("content", Arrays.asList(new String[]{"Red", "Blue", "Yellow", "Green"}));
        System.out.println(st.render());
    }

    public static void test7() {
        STGroup group = new STGroupFile("test7.stg", '$', '$');
        ST st = group.getInstanceOf("table");
        String[][] c = new String[][] {{"Top left", "Top right"},{"Bottom left", "Bottom right"}};
        st.add("cells", c); 
        System.out.println(st.render());

        ST st2 = group.getInstanceOf("table");

        ArrayList<ArrayList<String>> c2 = new ArrayList<ArrayList<String>>() {
            {
                add(new ArrayList<String>() {
                    {
                        add("One");
                        add("Two");
                    }
                });
                add(new ArrayList<String>() {
                    {
                        add("Three");
                        add("Four");
                    }
                });
                add(new ArrayList<String>() {
                    {
                        add("Five");
                        add("Six");
                    }
                });
            }
        };
        st2.add("cells", c2); 
        System.out.println(st2.render());
    }

    static class Pair {
        String a;
        String b;

        Pair(String a, String b) {
            this.a = a;
            this.b = b;
        }

        public String getA() {return a;}
        public String getB() {return b;}

        public String toString() {return "("+a+", "+b+")";}
    }

    public static void test8() {
        ST st = new ST("a pair: <$p.A$,$p.B$>", '$', '$');
        st.add("p", new Pair("Gustav", "Helge"));
        System.out.println(st.render());
    }

    public static void test9() {
        STGroup group = new STGroupFile("test9.stg", '$', '$');
        ST st = group.getInstanceOf("structDecl");
        st.add("name", "testStruct");
        st.addAggr("decls.{type, name}", "int", "foo");
        st.addAggr("decls.{type, name}", "long", "bar");
        st.addAggr("decls.{type, name}", "boolean", "sna");
        System.out.println(st.render());

        ST st2 = group.getInstanceOf("varDecls");
        st2.addAggr("decls.{type, name, initval}", "int", "foo", "17");
        st2.addAggr("decls.{type, name, initval}", "long", "bar", "4200");
        st2.addAggr("decls.{type, name, initval}", "float", "baz", null);
        st2.addAggr("decls.{type, name, n/a}", "boolean", "sna", "this is ignored");
        System.out.println(st2.render());
    }

    public static void main(String a[]){
        test1();
        test2();
        test3();
        test4();
        test5();
        test6();
        test7();
        test8();
        test9();
    }
}
