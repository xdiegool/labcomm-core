package se.lth.control.labcomm2014;

import java.io.PrintStream;
import java.util.List;
import java.util.LinkedList;
import java.util.Iterator;

public class SigStructType extends DataType {
    private List<SigField> fields;

    SigStructType(int nSigFields) {
        super("struct", Constant.STRUCT);
        this.fields = new LinkedList<SigField>();
    }

    public SigStructType() {
        this("struct");
    }

    protected SigStructType(String name) {
        super(name, Constant.STRUCT);
        this.fields = new LinkedList<SigField>();
    }

    public SigField[] getFields() {
        return fields.toArray(new SigField[0]);
    }

    public Iterator<SigField> getFieldIterator() {
        return fields.iterator();
    }

    public void addField(String name, DataType type) {
        fields.add(new SigField(name, type));
    }

    public void addField(SigField f) {
        fields.add(f);
    }

    public boolean isStruct() {
        return true;
    }

    public boolean isVoid() {
        return fields.size() == 0;
    }

    public String toString() {
        if(isVoid()) { //void type is empty struct
            return "void";
        } else {
            StringBuilder sb = new StringBuilder();
            sb.append("struct {\n");
            for(SigField f : fields) {
                sb.append(f.toString());
                sb.append(";\n");
            }
            sb.append("}");
            return sb.toString();
        }
    }

    public void accept(SignatureSymbolVisitor v) {
        v.visit(this);
    }
    public void print(PrintStream out, String indent) {
        out.println("struct {");
        String newIndent=indent+"    ";
        out.print(newIndent);
        Iterator<SigField> it = getFieldIterator();
        while(it.hasNext()) {
            it.next().print(out, newIndent);
            if(it.hasNext()) {
                out.print(newIndent);
            }
        }
        out.print(indent+"}");
    }
}

