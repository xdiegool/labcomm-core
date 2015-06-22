package se.lth.control.labcomm2014;

public class SigUserType extends DataType {

    public String toString() {
        return getName();
    }

    public SigUserType(String name) {
        super(name, 0);
    }

    public boolean isUserType() {
        return true;
    }

    public void accept(SignatureSymbolVisitor v) {
        v.visit(this);
    }
}


