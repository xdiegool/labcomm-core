package se.lth.control.labcomm2014;
public class SampleSymbol implements SignatureSymbol {
    public String toString() {
        return "sample ";
    }
    public void accept(SignatureSymbolVisitor v){
        v.visit(this);
    }
}

