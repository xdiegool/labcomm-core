package se.lth.control.labcomm2014;
import java.io.PrintStream;

public class SigArrayType extends DataType {
    private int idx[];
    private DataType type;

    public SigArrayType(DataType elementType, int idx[]) {
        super("array", Constant.ARRAY);
        this.idx = idx;
        this.type = elementType;
    }

    public DataType getType() {
        return type;
    }

    public int[] getIdx() {
        return idx;
    }

    public boolean isArray() {
        return true;
    }

    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(type.toString());
        for(int i : idx) {
            sb.append("["+(i>0 ? i : "_")+"]");
        }
        return sb.toString();
    }

    public void accept(SignatureSymbolVisitor v) {
        v.visit(this);
    }

    public void print(PrintStream out, String indent) {
        type.print(out,indent);
        out.print("[");
        for(int i =0; i<idx.length;i++) {
            int d = idx[i];
            out.print( (d==-1?"-":d));
            if(i<idx.length-1) {
                out.print(", ");
            }
        }
        out.print("]");
    }
}

