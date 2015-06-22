package se.lth.control.labcomm2014;
import java.io.PrintStream;
import java.util.Iterator;

public abstract class DataType implements SignatureSymbol{

    private final String name;
    private final int typeTag;
    public static String tagName(int typeTag) {
        switch(typeTag) {
            case Constant.BOOLEAN : return "boolean";
            case Constant.BYTE : return "byte";
            case Constant.SHORT : return "short";
            case Constant.INT : return "int";
            case Constant.LONG : return "long";
            case Constant.FLOAT : return "float";
            case Constant.DOUBLE : return "double";
            case Constant.STRING : return "string";
            case Constant.SAMPLE : return "sample";
        }
        throw new Error("not primitive type tag : " +Integer.toHexString(typeTag));
    }
    protected DataType(String name, int typeTag) {
        this.name = name;
        this.typeTag = typeTag;
    }

//    protected DataType(int typeTag) {
//        this(tagName(typeTag), typeTag);
//    }
//

    public String getName() {
        return name;
    }

    public int getTag() {
        return typeTag;
    }
    public boolean isArray() {
        return false;
    }
    public boolean isStruct(){
        return false;
    }
    public boolean isUserType(){
        return false;
    }

    public void addField(String name, DataType type) {
        throw new Error("cannot add field to "+getClass().getSimpleName());
    }

    public final void print(PrintStream out) {
        print(out, "");
    }

    public void print(PrintStream out, String indent) {
        out.print(name);
    }


    private static void printDependencies(PrintStream out, SampleDispatcher sd, String indent) throws java.io.IOException {
        Iterator<SampleDispatcher> it = sd.getDependencyIterator();
        if(it.hasNext()) {
            out.println(indent+"dependencies:");
        }
        while(it.hasNext()){
            SampleDispatcher d = it.next();
            printDataType(out, d, indent);
            out.println(indent+"---");
        }
    }

    public static void printDataType(PrintStream out, SampleDispatcher sd, String indent) throws java.io.IOException{
        out.print(indent+"typedef ");
        sd.getDataType().print(out,indent);
        out.println(" "+sd.getName()+";");
        printDependencies(out, sd, "...."+indent);
    }

    public static void printDataType(PrintStream out, SampleDispatcher sd) throws java.io.IOException{
        printDataType(out, sd, "");
    }
    private static void printDependencies(PrintStream out, SigTypeDef td, String indent) throws java.io.IOException {
        Iterator<SigTypeDef> it = td.getDepIterator();
        if(it.hasNext()) {
            out.println(indent+"dependencies:");
        }
        while(it.hasNext()){
            SigTypeDef d = it.next();
            printDataType(out, d, indent);
            out.println(indent+"---");
        }
    }

    public static void printDataType(PrintStream out, SigTypeDef d, String indent) throws java.io.IOException{
        if(d==null) {
            System.out.println("********* WARNING: printDataType(null)???");
            return;
        }
        out.print(indent+d.defType()+" ");
        d.getType().print(out,indent);
        out.println(" "+d.getName()+";");
        printDependencies(out, d, "...."+indent);
    }
    public static void printDataType(PrintStream out, SigTypeDef d) throws java.io.IOException{
        printDataType(out, d, "");
    }

}
