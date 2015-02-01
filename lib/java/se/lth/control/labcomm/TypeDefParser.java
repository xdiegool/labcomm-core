package se.lth.control.labcomm;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.EOFException;

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
        TypeDef td;
        if(d.getTypeIndex() == Constant.TYPE_BIND_SELF){
            //TODO: make the sample_def signature a TypeDef.
            // e.g., by looking up the signature in the decoder
            // (how to expose that?)
            //td = typeDefs.get(d.getTypeIndex());
           
            //XXX this will return a SelfBinding (w/o a signature)
            td = getTypeDefForIndex(d.getSampleIndex());
        } else {
            typeBindings.put(d.getSampleIndex(), d.getTypeIndex());
            td = getTypeDefForIndex(d.getSampleIndex());
        }
        
        Iterator<TypeDefListener> it = listeners.iterator();
        while(it.hasNext()){
            it.next().onTypeDef(td);
            //it.next().onTypeDef(typeDefs.get(d.getTypeIndex()));
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


///// parsing
//
//

    private class ParserState {
        private ByteArrayInputStream bis;
        private DataInputStream in;

        private LinkedList<Integer> typeStack;


        private ParserState() {
            typeStack = new LinkedList<Integer>();
        }

        public ParserState(int typeIdx) {
            this();
            pushType(typeIdx);
        }
         
        public ParserState(byte sig[]) {
            this();
            bis= new ByteArrayInputStream(sig);
            in = new DataInputStream(bis);
        }

        public void pushType(int typeIdx) {
            if(typeIdx >= 0x40 && !typeStack.contains(typeIdx)) {
                typeStack.push(typeIdx);
            } else {
                //throw new Error("typeIdx < 0x40");
            }
        }

        public void popType() {
            int tid = typeStack.pop();
            TypeDef td2 = typeDefs.get(tid);
            System.out.println(td2.getName());
            bis =new ByteArrayInputStream(td2.getSignature());
            in = new DataInputStream(bis);
        }

        public boolean moreTypes() {
            return !typeStack.isEmpty();
        }
        public String decodeString() throws IOException {
            int len = decodePacked32() & 0xffffffff;
            byte[] chars = new byte[len];
            for(int i=0; i<len; i++) {
                chars[i] = in.readByte();
            }
            return new String(chars);
        }

        public int decodePacked32() throws IOException {
            long res=0;
            byte i=0;
            boolean cont=true;

            do {
            byte c = in.readByte();
            res = (res << 7) | (c & 0x7f);
            cont = (c & 0x80) != 0;
            i++;
            } while(cont);

            return (int) (res & 0xffffffff);
        }
    }

//    public HierarchicalTypeDef parseSignature(int typeIndex) {
    public void parseSignature(int typeIndex) throws IOException{
//    public void parseSignature(TypeDef td) throws IOException{

        //TypeDef td = getTypeDefForIndex(sampleIndex);

//        byte sig[] = td.getSignature();

//        ParserState s = new ParserState(sig);
        ParserState s = new ParserState(typeIndex);

        try {
      //      parseType(s);

            while(s.moreTypes()) {
                s.popType();
                parseType(s);
            }

        } catch(java.io.EOFException ex) {
            System.out.println("EOF: self_binding");
        }
    }    

    private void parseArray(ParserState in) throws IOException {
        int numIdx = in.decodePacked32();
        int idx[] = new int[numIdx];
        for(int i=0; i<numIdx; i++){
            idx[i] = in.decodePacked32(); 
            System.out.println(idx[i]);
        }
        int type = in.decodePacked32();
        lookupType(type, in); 
    }

    private void parseStruct(ParserState in) throws IOException {
        System.out.println("struct");
        int numFields = in.decodePacked32();
        for(int i=0; i<numFields; i++) {
            parseField(in);
        }
    }

    private void parseField(ParserState in) throws IOException {
        String name = in.decodeString();
        System.out.print("    "+name+" : ");
        parseType(in);
    }

    private void lookupType(int tag, ParserState in) {
        switch(tag) {
            case Constant.BOOLEAN:
                System.out.println("boolean");
                break;
            case Constant.BYTE:
                System.out.println("byte");
                break;
            case Constant.SHORT:
                System.out.println("short");
                break;
            case Constant.INT:
                System.out.println("int");
                break;
            case Constant.LONG:
                System.out.println("long");
                break;
            case Constant.FLOAT:
                System.out.println("float");
                break;
            case Constant.DOUBLE:
                System.out.println("double");
                break;
            case Constant.STRING:
                System.out.println("string");
                break;
            default:
                {
                    TypeDef td = typeDefs.get(tag);
                    System.out.println(td.getName());
                }
                in.pushType(tag);
                break;
        }
    }

    private void parseType(ParserState in) throws IOException {
        int tag = in.decodePacked32();
        switch(tag) {
            case 0:
                System.out.println("SELF");
                break;
            case Constant.ARRAY:
                parseArray(in);
                break;
            case Constant.STRUCT:
                parseStruct(in);
                break;
            default:
                lookupType(tag, in);
                break;
        }    
    }

}


