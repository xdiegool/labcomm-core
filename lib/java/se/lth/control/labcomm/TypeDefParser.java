package se.lth.control.labcomm;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;

import se.lth.control.labcomm.Decoder;
import se.lth.control.labcomm.TypeDef;
import se.lth.control.labcomm.TypeBinding;

public class TypeDefParser implements TypeDef.Handler, TypeBinding.Handler {

    static class SelfBinding extends TypeDef {

        private byte[] dummy = new byte[0];
        public String getName() {return "self";} 
        public int getIndex() {return 0;}
        public byte[] getSignature() {return dummy;}
    }

    public interface TypeDefListener {
        void onTypeDef(TypeDef d);
    }

    private HashMap<Integer,TypeDef> typeDefs;
    private HashMap<Integer,Integer> typeBindings;
    private HashSet<TypeDefListener> listeners;

    protected TypeDefParser() {
        typeDefs = new HashMap<Integer,TypeDef>();
        typeBindings = new HashMap<Integer,Integer>();
        listeners = new HashSet<TypeDefListener>();

        typeDefs.put(0, new SelfBinding());
    }

    public void addListener(TypeDefListener l) {
        listeners.add(l);
    }

    public void handle_TypeDef(TypeDef d) throws java.io.IOException {
        //System.out.println("Got TypeDef: "+d.getName()+"("+d.getIndex()+")");
        typeDefs.put(d.getIndex(), d);
    }

    public void handle_TypeBinding(TypeBinding d) throws java.io.IOException {
        //System.out.println("TDP got TypeBinding: "+d.getSampleIndex()+" --> "+d.getTypeIndex()+"");
        typeBindings.put(d.getSampleIndex(), d.getTypeIndex());
        
        Iterator<TypeDefListener> it = listeners.iterator();
        while(it.hasNext()){
            //it.next().onTypeDef(getTypeDefForIndex(d.getSampleIndex()));
            it.next().onTypeDef(typeDefs.get(d.getTypeIndex()));
        }
    }

    //* temporary testing method */
    public TypeDef getTypeDefForIndex(int sampleIndex) {
        return typeDefs.get(typeBindings.get(sampleIndex));
    }


    /** Factory method
     *  @return a new TypeDefParser registered on d
     */
    public static TypeDefParser registerTypeDefParser(Decoder d) throws java.io.IOException  {

        TypeDefParser res = new TypeDefParser();

        TypeDef.register(d,res);
        TypeBinding.register(d,res);

        return res;
    }
}


