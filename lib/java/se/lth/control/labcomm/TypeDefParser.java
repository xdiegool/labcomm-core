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
import se.lth.control.labcomm.DecoderChannel;
import se.lth.control.labcomm.TypeDef;
import se.lth.control.labcomm.TypeBinding;


public class TypeDefParser implements TypeDef.Handler, TypeBinding.Handler {

    static class SelfBinding extends TypeDef {

        private int sampleIndex;
        private Decoder decoder;
        private byte[] dummy = new byte[0];
        public String toString() {return "self";} 
        public String getName() {
            if(decoder instanceof DecoderChannel) {
                DecoderChannel dc = (DecoderChannel) decoder;
                return dc.getSampleName(sampleIndex);
            } else {
                return "self";
            }
        } 
        public int getIndex() {return 0;}
        public byte[] getSignature() {
            //todo: get sample signature from decoder
            //return dummy;

            if(decoder instanceof DecoderChannel) {
                DecoderChannel dc = (DecoderChannel) decoder;
                return dc.getSampleSignature(sampleIndex);
            } else {
                return dummy;
            }
        }

        public SelfBinding(int sampleIndex, Decoder decoder) {
            super();
            this.sampleIndex = sampleIndex;
            this.decoder = decoder;
        }
    }

    public interface TypeDefListener {
        //void onTypeDef(TypeDef d);
        void onTypeDef(ParsedTypeDef d);
    }

    private HashMap<Integer,TypeDef> typeDefs;
    private HashMap<Integer,Integer> typeBindings;
    private HashSet<TypeDefListener> listeners;
    private Decoder decoder;

    protected TypeDefParser(Decoder d) {
        this.decoder = d;
        typeDefs = new HashMap<Integer,TypeDef>();
        typeBindings = new HashMap<Integer,Integer>();
        listeners = new HashSet<TypeDefListener>();

        //typeDefs.put(0, new SelfBinding());
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
            // (how to expose that? A good option may be to
            // make this internal to the Decoder, and just expose a
            // Parsed{Sample,Type}Def for user code to register handlers for)
             td = new SelfBinding(d.getSampleIndex(), decoder);
           
            //XXX this will return a SelfBinding (w/o a signature)
            //td = getTypeDefForIndex(d.getSampleIndex());
        } else {
            typeBindings.put(d.getSampleIndex(), d.getTypeIndex());
            td = getTypeDefForIndex(d.getSampleIndex());
            //System.out.println("handleTypeBinding: td:"+td.getIndex()+"=="+td.getName());
        }
        
        //ParsedSampleDef result = parseSignature(d.getSampleIndex());
        ParsedSampleDef result = parseSignature(td);

        Iterator<TypeDefListener> it = listeners.iterator();
        while(it.hasNext()){
            //it.next().onTypeDef(result);            
            notifyListener(it.next(), result);
        }
    }

    private void notifyListener(TypeDefListener l, ParsedTypeDef d) {
        l.onTypeDef(d);
        if(d instanceof ParsedSampleDef) {
            for(ParsedTypeDef dep : ((ParsedSampleDef)d).getDependencies()) {
                //do we want to change ParseTypeDef to have dependencies,
                //and do recursion here?
                //notifyListener(l, dep);
                l.onTypeDef(dep);
            }
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

        TypeDefParser res = new TypeDefParser(d);

        TypeDef.register(d,res);
        TypeBinding.register(d,res);

        return res;
    }


///// parsing
//
//

// Sketch of result types "unparsed labcomm-file"
//   
// 
    public abstract class ParsedType{
    }

    public class PrimitiveType extends ParsedType {
        //TODO: instead, store tag and lookup in Constant.
        private String name;
        public PrimitiveType(String name) {
            this.name = name;
        }

        public String toString() { return name;}
    }

    public class StructType extends ParsedType {
        private Field fields[];

        public StructType(int nFields) {
            this.fields = new Field[nFields];
        }

        public void setField(int idx, Field f) {
            fields[idx] = f;
        }

        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append("struct {\n");
            for(Field f : fields) {
                sb.append(f.toString());
                sb.append(";\n");        
            }
            sb.append("}");
            return sb.toString();
        }
    }

    public class Field {
        private ParsedType type;
        private String name;

        public Field(String name, ParsedType type) {
            this.name = name;
            this.type = type;
        }

        public String toString() {
            return type.toString() + " " + name;
        }
    }

    public class ArrayType extends ParsedType {
        private int idx[];
        private ParsedType type;

        public ArrayType(int idx[], ParsedType elementType) {
            this.idx = idx;
            this.type = elementType;
        }

        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append(type.toString());
            for(int i : idx) {
                sb.append("["+(i>0 ? i : "_")+"]");
            }
            return sb.toString();
        }
    }

    public class UserType extends ParsedType {
        private String name;
        public String toString() {
            return name;
        }

        public UserType(String name) {
            this.name = name;
        }
    }

    public class ParsedTypeDef{
       private int idx;
       private String name;
       private ParsedType type;

       public ParsedTypeDef(int idx, String name){
            this.idx = idx;
            this.name = name;
       }

       public ParsedTypeDef(int idx, String name, ParsedType type) {
           this(idx, name);
           this.type = type;
       }

       public int hashCode() {
           return name.hashCode();
       }

       public boolean equals(Object o) {
            if(! (o instanceof ParsedTypeDef)){
                return false;
            } else {
                ParsedTypeDef other = (ParsedTypeDef) o;
                return other.idx == idx && other.name.equals(name);
            }
       }
            
            

       public void setType(ParsedType type) {
           this.type = type;
       }

       public ParsedType getType() {
           return type;
       }

       public int getIndex() {
           return idx;
       }

       public String getName() {
            return name;
       }

       public String toString() {
          return type.toString();
       }
    }
   
    public class ParsedSampleDef extends ParsedTypeDef{

        private HashSet<ParsedTypeDef> deps;
        public ParsedSampleDef(ParsedTypeDef td) {
            super(td.getIndex(), td.getName(), td.getType());
            this.deps = new HashSet<ParsedTypeDef>();
        }

        public void addDependency(ParsedTypeDef d) {
            deps.add(d);
        }
        private HashSet<ParsedTypeDef> getDependencies() {
            return deps;
        }
    }

    private class ParserState {
        private ByteArrayInputStream bis;
        private DataInputStream in;
        private TypeDef current;
        private ParsedTypeDef currentParsed;

        private LinkedList<TypeDef> typeStack;

        public ParsedTypeDef newTypeDef() {
            currentParsed =new ParsedTypeDef(getCurrentIndex(), getCurrentName());
            return currentParsed;
        }

        private ParserState() {
            typeStack = new LinkedList<TypeDef>();
        }

        public ParserState(int typeIdx) {
            this();
            pushType(typeIdx);
        }
         
        public ParserState(TypeDef td) {
            this();
            pushType(td);
        }
         
        public ParserState(byte sig[]) {
            this();
            bis= new ByteArrayInputStream(sig);
            in = new DataInputStream(bis);
        }

        public void pushType(TypeDef td) {
            if(!typeStack.contains(td)) {
                typeStack.push(td);
            }
        }
        public void pushType(int typeIdx) {
            if(typeIdx >= 0x40 && !typeStack.contains(typeIdx)) {
                //typeStack.push(typeIdx);
                typeStack.push(typeDefs.get(typeIdx));
            } else {
                //throw new Error("typeIdx < 0x40");
            }
        }

        public void popType() {
            //int tid = typeStack.pop();
            //TypeDef td2 = typeDefs.get(tid);
            TypeDef td2 = typeStack.pop();
            current = td2;
            //if(td2 != null ) {
            //    System.out.println(td2.getName());
            //} else {
            //    System.out.println("popType: null for idx "+tid);
            //}
            bis =new ByteArrayInputStream(td2.getSignature());
            in = new DataInputStream(bis);
        }

        public boolean moreTypes() {
            return !typeStack.isEmpty();
        }

        public int getCurrentIndex() {
            return current.getIndex();
        }

        public String getCurrentName() {
            return current.getName();
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

    public ParsedSampleDef parseSignature(TypeDef td) throws IOException{
//    public void parseSignature(int typeIndex) throws IOException{

        int typeIndex = td.getIndex();
        //System.out.println("parseSignature :"+td);
        //ParserState s = new ParserState(typeIndex);
        ParserState s = new ParserState(td);

        ParsedSampleDef result=null;
        try {
            s.popType();
            result = parseSampleTypeDef(s);
            while(s.moreTypes()) {
                s.popType();
                result.addDependency(parseTypeDef(s));
            }
        } catch(java.io.EOFException ex) {
            System.out.println("EOF: self_binding");
        }
        return result;
    }    

    private ArrayType  parseArray(ParserState in) throws IOException {
        int numIdx = in.decodePacked32();
        int idx[] = new int[numIdx];
        for(int i=0; i<numIdx; i++){
            idx[i] = in.decodePacked32(); 
            //System.out.println(idx[i]);
        }
        int type = in.decodePacked32();
        ParsedType elementType = lookupType(type, in); 
        ArrayType result = new ArrayType(idx, elementType);
        for(int i=0; i<numIdx; i++){
            idx[i] = in.decodePacked32(); 
        }
        return result;
    }

    private StructType parseStruct(ParserState in) throws IOException {
        //System.out.println("struct");
        int numFields = in.decodePacked32();
        StructType result = new StructType(numFields);
        for(int i=0; i<numFields; i++) {
            result.setField(i, parseField(in));
        }
        return result;
    }

    private Field parseField(ParserState in) throws IOException {
        String name = in.decodeString();
        return new Field(name, parseType(in));
    }

    private ParsedType lookupType(int tag, ParserState in) {
        ParsedType result = null;
        switch(tag) {
            case Constant.BOOLEAN:
                result = new PrimitiveType("boolean");
                break;
            case Constant.BYTE:
                result = new PrimitiveType("byte");
                break;
            case Constant.SHORT:
                result = new PrimitiveType("short");
                break;
            case Constant.INT:
                result = new PrimitiveType("int");
                break;
            case Constant.LONG:
                result = new PrimitiveType("long");
                break;
            case Constant.FLOAT:
                result = new PrimitiveType("float");
                break;
            case Constant.DOUBLE:
                result = new PrimitiveType("double");
                break;
            case Constant.STRING:
                result = new PrimitiveType("string");
                break;
            default:
                {
                    TypeDef td = typeDefs.get(tag);
                    result = new UserType(td.getName());
                }
                in.pushType(tag);
                break;
        }
        return result;
    }

    private ParsedSampleDef parseSampleTypeDef(ParserState in) throws IOException {
        ParsedTypeDef td = parseTypeDef(in);
        return new ParsedSampleDef(td);
    }
    private ParsedTypeDef parseTypeDef(ParserState in) throws IOException {
        ParsedTypeDef result = in.newTypeDef();
        result.setType(parseType(in));
        return result;
    }
    private ParsedType parseType(ParserState in) throws IOException {
        int tag = in.decodePacked32();
        ParsedType result = null;
        switch(tag) {
            case 0:
                System.out.println("SELF");
                break;
            case Constant.ARRAY:
                result = parseArray(in);
                break;
            case Constant.STRUCT:
                result = parseStruct(in);
                break;
            default:
                result = lookupType(tag, in);
                break;
        }    
        return result;
    }

}
