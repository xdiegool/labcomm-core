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
            //it.next().onTypeDef(td);
            it.next().onTypeDef(result);            
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
    public class ParsedTypeDef{
       private int idx;
       private String name;
       private String lcDecl;
       private StringBuilder sb;

       public ParsedTypeDef(int idx, String name){
            this.idx = idx;
            this.name = name;
            this.sb = new StringBuilder();
       }

       public int getIndex() {
           return idx;
       }

       public String getName() {
            return name;
       }

       public String toString() {
          //return "TODO: pretty-print typedef"; 
          return "FOOO: toString"+sb.toString();
       }

       public void addType(String type) {
           System.out.println("addType: "+type);
           sb.append(type);
           sb.append("\n");
       }
       public void addFieldName(String name) {
           System.out.println("addFieldName: "+name);
           sb.append(name);
           sb.append("\n");
       }
       public void addDim(String dim) {
            System.out.println("addDim: "+dim);
           sb.append(dim);
           sb.append("\n");
       }
    }
   
    public class ParsedSampleDef extends ParsedTypeDef{

        private HashSet<ParsedTypeDef> deps;
        public ParsedSampleDef(ParsedTypeDef td) {
            super(td.getIndex(), td.getName());
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

        public void addType(String type) {
            currentParsed.addType(type);
        }

        public void addFieldName(String name) {
            currentParsed.addFieldName(name);
        }

        public void addDim(String dim) {
            currentParsed.addDim(dim);
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
        System.out.println("parseSignature :"+td);
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

    private void parseArray(ParserState in) throws IOException {
        in.addType("array");
        int numIdx = in.decodePacked32();
        int idx[] = new int[numIdx];
        for(int i=0; i<numIdx; i++){
            idx[i] = in.decodePacked32(); 
            //System.out.println(idx[i]);
        }
        int type = in.decodePacked32();
        lookupType(type, in); 
        for(int i=0; i<numIdx; i++){
            idx[i] = in.decodePacked32(); 
            in.addDim( (idx[i] == 0 ? "_" : "0x"+Integer.toHexString(idx[i])));
        }
    }

    private void parseStruct(ParserState in) throws IOException {
        //System.out.println("struct");
        in.addType("struct");
        int numFields = in.decodePacked32();
        for(int i=0; i<numFields; i++) {
            parseField(in);
        }
    }

    private void parseField(ParserState in) throws IOException {
        String name = in.decodeString();
        in.addFieldName("    "+name+" : ");
        parseType(in);
    }

    private void lookupType(int tag, ParserState in) {
        switch(tag) {
            case Constant.BOOLEAN:
                in.addType("boolean");
                break;
            case Constant.BYTE:
                in.addType("byte");
                break;
            case Constant.SHORT:
                in.addType("short");
                break;
            case Constant.INT:
                in.addType("int");
                break;
            case Constant.LONG:
                in.addType("long");
                break;
            case Constant.FLOAT:
                in.addType("float");
                break;
            case Constant.DOUBLE:
                in.addType("double");
                break;
            case Constant.STRING:
                in.addType("string");
                break;
            default:
                {
                    TypeDef td = typeDefs.get(tag);
                    //in.addType(td.getName());
                }
                in.pushType(tag);
                break;
        }
    }

    private ParsedSampleDef parseSampleTypeDef(ParserState in) throws IOException {
        ParsedTypeDef td = parseTypeDef(in);
        return new ParsedSampleDef(td);
    }
    private ParsedTypeDef parseTypeDef(ParserState in) throws IOException {
        ParsedTypeDef result = in.newTypeDef();
        parseType(in);
        return result;
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
