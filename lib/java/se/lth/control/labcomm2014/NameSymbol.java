package se.lth.control.labcomm2014;
public class NameSymbol implements SignatureSymbol {
    private String name;

    public NameSymbol(String name) {
        this.name = name;
    }

    public String toString() {
        return name;
    }
    public void accept(SignatureSymbolVisitor v){
        v.visit(this);
    }
}

