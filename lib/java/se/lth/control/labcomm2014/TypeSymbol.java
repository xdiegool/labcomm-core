package se.lth.control.labcomm2014;

public class TypeSymbol implements SignatureSymbol {
    public String toString() {
        return "typedef ";
    }
    public void accept(SignatureSymbolVisitor v){
        v.visit(this);
    }
}

