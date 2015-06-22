package se.lth.control.labcomm2014;

public class SigPrimitiveType extends DataType {

    public SigPrimitiveType(int tag) {
        super(tagName(tag), tag);
    }

    public void accept(SignatureSymbolVisitor v) {
        v.visit(this);
    }

    public String toString() {
        return getName();}
}

