package se.lth.control.labcomm2014;
import java.util.HashSet;
import java.util.Iterator;

public class SigTypeDef{
    private String name;
    private DataType type;
    protected HashSet<SigTypeDef> deps;

    SigTypeDef(SigTypeDef td) {
        this(td.getName(), td.getType());
        this.deps = new HashSet<SigTypeDef>(td.deps);
    }

    SigTypeDef( String name){
        this.name = name;
        this.deps = new HashSet<SigTypeDef>();
    }

    SigTypeDef(String name, DataType type) {
        this(name);
        this.type = type;
    }

    void addDependency(SigTypeDef d) {
        deps.add(d);
    }

    HashSet<SigTypeDef> getDependencies() {
        return deps;
    }

    Iterator<SigTypeDef> getDepIterator() {
        return deps.iterator();
    }
    /** To be overridden in SigSampleDef
    */
    public boolean isSampleDef() {
        return false;
    }

    public String defType() {
        return "typedef";
    }

    void setType(DataType type) {
        this.type = type;
    }

    public DataType getType() {
        if(type==null) {
            System.out.println("******** WARNING! SigTypeDef.getType returning null");
        }
        return type;
    }

    int getIndex() {
        return 0;
    }

    public String getName() {
        return name;
    }

    public String toString() {
        return type.toString();
    }

    public int hashCode() {
        return name.hashCode();
    }

    public boolean equals(Object o) {
        if(! (o instanceof SigTypeDef)){
            return false;
        } else {
            SigTypeDef other = (SigTypeDef) o;
            return other.getIndex() == getIndex() && other.name.equals(name);
        }
    }

    public void accept(SignatureSymbolVisitor v) {
        type.accept(v);
    }
}


