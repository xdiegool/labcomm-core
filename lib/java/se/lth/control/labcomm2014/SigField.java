package se.lth.control.labcomm2014;
import java.io.PrintStream;

public class SigField implements SignatureSymbol{
    private DataType type;
    private String name;

    SigField(String name, DataType type) {
        this.name = name;
        this.type = type;
    }

    public DataType getType() {
        return type;
    }

    public String getName() {
        return name;
    }

    public String toString() {
        return type.toString() + " " + name;
    }

    public void accept(SignatureSymbolVisitor v) {
        v.visit(this);
    }

    public void print(PrintStream out, String indent) {
        type.print(out, indent);
        out.println(" "+name+";");
    }
}


