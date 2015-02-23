package se.lth.control.labcomm;

import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Iterator;
import java.util.NoSuchElementException;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.EOFException;

import se.lth.control.labcomm.Decoder;
import se.lth.control.labcomm.DecoderChannel;
import se.lth.control.labcomm.TypeDef;
import se.lth.control.labcomm.TypeBinding;

public class TypeDefParser implements TypeDef.Handler, TypeBinding.Handler {

    public interface TypeDefListener {
        void onTypeDef(ParsedTypeDef d);
    }

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

    static class EmptyIterator<T>  implements java.util.Iterator {
        @Override
        public boolean hasNext() {
            return false;
        }

        @Override
        public T next() {
            throw new NoSuchElementException();
        }

        public void remove() {
            throw new UnsupportedOperationException();
        }
    }

    static EmptyIterator<ParsedTypeDef> emptyIterator =
        new EmptyIterator<>();

    private HashMap<Integer,TypeDef> typeDefs;
    private HashMap<Integer,Integer> typeBindings;
    private HashSet<TypeDefListener> listeners;
    private LinkedList<ParsedSampleDef> sampleDefs;
    private Decoder decoder;

    protected TypeDefParser(Decoder d) {
        this.decoder = d;
        typeDefs = new HashMap<Integer,TypeDef>();
        typeBindings = new HashMap<Integer,Integer>();
        listeners = new HashSet<TypeDefListener>();
        sampleDefs = new LinkedList<ParsedSampleDef>();
    }

    public void addListener(TypeDefListener l) {
        listeners.add(l);
    }


    public Iterator<ParsedSampleDef> sampleDefIterator() {
        return sampleDefs.iterator();
    }

    public void handle_TypeDef(TypeDef d) throws java.io.IOException {
        typeDefs.put(d.getIndex(), d);
    }

    public void handle_TypeBinding(TypeBinding d) throws java.io.IOException {
        TypeDef td;
        if(d.isSelfBinding()){
             td = new SelfBinding(d.getSampleIndex(), decoder);
        } else {
            typeBindings.put(d.getSampleIndex(), d.getTypeIndex());
            td = getTypeDefForIndex(d.getSampleIndex());
        }
        ParsedSampleDef result = parseSignature(td);

        sampleDefs.add(result);

        Iterator<TypeDefListener> it = listeners.iterator();
        while(it.hasNext()){
            notifyListener(it.next(), result);
        }
    }

    private void notifyListener(TypeDefListener l, ParsedTypeDef d) {
        l.onTypeDef(d);
        if(d instanceof ParsedSampleDef) {
            for(ParsedTypeDef dep : ((ParsedSampleDef)d).getDependencies()) {
                //do we want to change ParseTypeDef to have dependencies,
                //and do recursion here?
                //if so, do notifyListener(l, dep);
                l.onTypeDef(dep);
            }
        }
    }

    private TypeDef getTypeDefForIndex(int sampleIndex) {
        return typeDefs.get(typeBindings.get(sampleIndex));
    }


    /** Factory method for use by application programs:
     *  registers a TypeDefParser for handling TypeDef and TypeBinding
     *  on the Decoder d.
     *
     *  @return a new TypeDefParser registered on d
     */
    public static TypeDefParser registerTypeDefParser(Decoder d) throws java.io.IOException  {

        TypeDefParser res = new TypeDefParser(d);

        TypeDef.register(d,res);
        TypeBinding.register(d,res);

        return res;
    }

    public LinkedList<ParsedSymbol> symbolify() {

        LinkedList<ParsedSymbol> result = new LinkedList<ParsedSymbol>();

        Iterator<ParsedSampleDef> sdi = sampleDefIterator();

        while(sdi.hasNext()) {
            ParsedSampleDef sd = sdi.next();
            result.add(new SampleSymbol());
            result.add(sd.getType());
            result.add(new NameSymbol(sd.getName()));

            Iterator<ParsedTypeDef> di = sd.getDepIterator();        
            while(di.hasNext()) {
                ParsedTypeDef d = di.next();
                result.add(new TypeSymbol());
                result.add(d.getType());
                result.add(new NameSymbol(d.getName()));
            }
        }
        return result;
    }

    public String symbolString() {
        Iterator<ParsedSymbol> i = symbolify().iterator();

        StringBuilder sb = new StringBuilder();

        while(i.hasNext()) {
            sb.append(i.next().toString());
        }
        return sb.toString();
    }

    /* An interface for using Visitor pattern to traverse 
     * ParsedTypeDefs
     */
    public interface ParsedSymbolVisitor {
        void visit(TypeSymbol s);
        void visit(SampleSymbol s);
        void visit(NameSymbol s);
        void visit(PrimitiveType t);
        //sampleRefs are sent as primitive types
        //Put this back if that is changed to SampleRefType
        //void visit(SampleRefType t);
        void visit(ParsedStructType t);
        void visit(ParsedField t);
        void visit(ArrayType t);
        void visit(ParsedUserType t);
    }
    public abstract class ParsedSymbol{
        public abstract void accept(ParsedSymbolVisitor v);
    }

    public class TypeSymbol extends ParsedSymbol {
        public String toString() {
            return "typedef ";
        }
        public void accept(ParsedSymbolVisitor v){
            v.visit(this);
        }
    }

    public class SampleSymbol extends ParsedSymbol {
        public String toString() {
            return "sample ";
        }
        public void accept(ParsedSymbolVisitor v){
            v.visit(this);
        }
    }

    public class NameSymbol extends ParsedSymbol {
        private String name;

        public NameSymbol(String name) {
            this.name = name;
        }

        public String toString() { 
            return name;
        }
        public void accept(ParsedSymbolVisitor v){
            v.visit(this);
        }
    }

    public abstract class ParsedType extends ParsedSymbol{
    }

// SampleRefType currently not sent, se above
//    public class SampleRefType extends ParsedType {
//        public void accept(ParsedSymbolVisitor v) {
//            v.visit(this);
//        }
//
//        public String toString() { 
//            return "sample";}
//    }

    public class PrimitiveType extends ParsedType {
        private final String name;
        private int tag;

        String getName() {
            return name;
        }

        int getTag() {
            return tag;
        }
        PrimitiveType(int tag) {
            this.tag = tag;
            switch(tag) {
                case Constant.BOOLEAN:
                    this.name = "boolean";
                    break;
                case Constant.BYTE:
                    this.name = "byte";
                    break;
                case Constant.SHORT:
                    this.name = "short";
                    break;
                case Constant.INT:
                    this.name = "int";
                    break;
                case Constant.LONG:
                    this.name = "long";
                    break;
                case Constant.FLOAT:
                    this.name = "float";
                    break;
                case Constant.DOUBLE:
                    this.name = "double";
                    break;
                case Constant.STRING:
                    this.name = "string";
                    break;
                case Constant.SAMPLE:
                    this.name = "sample";
                    break;
                default:
                    this.name = "??? unknown tag 0x"+Integer.toHexString(tag);    
            }
        }

        public void accept(ParsedSymbolVisitor v) {
            v.visit(this);
        }

        public String toString() { 
            return name;}
    }

    public class ParsedStructType extends ParsedType {
        private ParsedField fields[];

        ParsedStructType(int nParsedFields) {
            this.fields = new ParsedField[nParsedFields];
        }

        public ParsedField[] getFields() {
            return fields;
        }

        void setParsedField(int idx, ParsedField f) {
            fields[idx] = f;
        }

        public boolean isVoid() {
            return fields.length == 0;
        }

        public String toString() {
            if(isVoid()) { //void type is empty struct
                return "void";
            } else {
                StringBuilder sb = new StringBuilder();
                sb.append("struct {\n");
                for(ParsedField f : fields) {
                    sb.append(f.toString());
                    sb.append(";\n");        
                }
                sb.append("}");
                return sb.toString();
            }
        }

        public void accept(ParsedSymbolVisitor v) {
            v.visit(this);
        }
    }

    public class ParsedField extends ParsedSymbol{
        private ParsedType type;
        private String name;

        ParsedField(String name, ParsedType type) {
            this.name = name;
            this.type = type;
        }

        public ParsedType getType() {
            return type;
        }

        public String getName() {
            return name;
        }

        public String toString() {
            return type.toString() + " " + name;
        }

        public void accept(ParsedSymbolVisitor v) {
            v.visit(this);
        }
    }

    public class ArrayType extends ParsedType {
        private int idx[];
        private ParsedType type;

        ArrayType(int idx[], ParsedType elementType) {
            this.idx = idx;
            this.type = elementType;
        }

        public ParsedType getType() {
            return type;
        }

        public int[] getIdx() {
            return idx;
        }

        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append(type.toString());
            for(int i : idx) {
                sb.append("["+(i>0 ? i : "_")+"]");
            }
            return sb.toString();
        }

        public void accept(ParsedSymbolVisitor v) {
            v.visit(this);
        }
    }

    public class ParsedUserType extends ParsedType {
        private String name;
        public String getName() {
            return name;
        }

        public String toString() {
            return name;
        }

        ParsedUserType(String name) {
            this.name = name;
        }

        public void accept(ParsedSymbolVisitor v) {
            v.visit(this);
        }
    }

    public class ParsedTypeDef{
       private int idx;
       private String name;
       private ParsedType type;
       private final TypeDefParser source;

       ParsedTypeDef(TypeDefParser source, int idx, String name){
           this.source = source;
           this.idx = idx;
           this.name = name;
       }

       ParsedTypeDef(TypeDefParser source, 
                     int idx, String name, ParsedType type) {
           this(source, idx, name);
           this.type = type;
       }

       ParsedTypeDef(int idx, String name){
           this(null, idx, name);
       }

       ParsedTypeDef(int idx, String name, ParsedType type) {
           this(idx, name);
           this.type = type;
       }

       /** @return true if source is same as
        */
       public boolean checkSource(TypeDefParser o) {
           boolean result = source == o;
           System.err.println("ParsedTypeDef.checkSource: source = "+source);
           System.err.println("ParsedTypeDef.checkSource: o "+o);
           System.err.println("ParsedTypeDef.checkSource: "+result);
           return result;
       }

       /** To be overridden in ParsedSampleDef
        */
       public boolean isSampleDef() {
           return false;
       }

        Iterator<ParsedTypeDef> getDepIterator() {
            return emptyIterator;  
        }        

       void setType(ParsedType type) {
           this.type = type;
       }

       ParsedType getType() {
           return type;
       }

       int getIndex() {
           return idx;
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
            if(! (o instanceof ParsedTypeDef)){
                return false;
            } else {
                ParsedTypeDef other = (ParsedTypeDef) o;
                return other.idx == idx && other.name.equals(name);
            }
       }

       public void accept(ParsedSymbolVisitor v) {
            type.accept(v);
       }
    }
   
    public class ParsedSampleDef extends ParsedTypeDef{

        private HashSet<ParsedTypeDef> deps;

        ParsedSampleDef(ParsedTypeDef td) {
            super(td.source, td.getIndex(), td.getName(), td.getType());
            this.deps = new HashSet<ParsedTypeDef>();
        }

        void addDependency(ParsedTypeDef d) {
            deps.add(d);
        }

        @Override
        public boolean isSampleDef() {
            return true;
        }
        private HashSet<ParsedTypeDef> getDependencies() {
            return deps;
        }

        Iterator<ParsedTypeDef> getDepIterator() {
            return deps.iterator();
        }        
    }

    private class ParserState {
        private ByteArrayInputStream bis;
        private DataInputStream in;
        private TypeDef current;
        private ParsedTypeDef currentParsed;

        private LinkedList<TypeDef> typeStack;

        ParsedTypeDef newTypeDef() {
            currentParsed =new ParsedTypeDef(TypeDefParser.this, getCurrentIndex(), getCurrentName());
            return currentParsed;
        }

        private ParserState() {
            typeStack = new LinkedList<TypeDef>();
        }

        ParserState(int typeIdx) {
            this();
            pushType(typeIdx);
        }
         
        ParserState(TypeDef td) {
            this();
            pushType(td);
        }
         
        ParserState(byte sig[]) {
            this();
            bis= new ByteArrayInputStream(sig);
            in = new DataInputStream(bis);
        }

        void pushType(TypeDef td) {
            if(!typeStack.contains(td)) {
                typeStack.push(td);
            }
        }
        void pushType(int typeIdx) {
            if(typeIdx >= 0x40 && !typeStack.contains(typeIdx)) {
                typeStack.push(typeDefs.get(typeIdx));
            } 
        }

        void popType() {
            TypeDef td2 = typeStack.pop();
            current = td2;
            bis =new ByteArrayInputStream(td2.getSignature());
            in = new DataInputStream(bis);
        }

        boolean moreTypes() {
            return !typeStack.isEmpty();
        }

        int getCurrentIndex() {
            return current.getIndex();
        }

        String getCurrentName() {
            return current.getName();
        }

        String decodeString() throws IOException {
            int len = decodePacked32() & 0xffffffff;
            byte[] chars = new byte[len];
            for(int i=0; i<len; i++) {
                chars[i] = in.readByte();
            }
            return new String(chars);
        }

        int decodePacked32() throws IOException {
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
        }
        int type = in.decodePacked32();
        ParsedType elementType = lookupType(type, in); 
        ArrayType result = new ArrayType(idx, elementType);
        for(int i=0; i<numIdx; i++){
            idx[i] = in.decodePacked32(); 
        }
        return result;
    }

    private ParsedStructType parseStruct(ParserState in) throws IOException {
        int numParsedFields = in.decodePacked32();
        ParsedStructType result = new ParsedStructType(numParsedFields);
        for(int i=0; i<numParsedFields; i++) {
            result.setParsedField(i, parseParsedField(in));
        }
        return result;
    }

    private ParsedField parseParsedField(ParserState in) throws IOException {
        String name = in.decodeString();
        return new ParsedField(name, parseType(in));
    }

    private ParsedType lookupType(int tag, ParserState in) {
        ParsedType result;
        if(tag >= Constant.FIRST_USER_INDEX) {
                TypeDef td = typeDefs.get(tag);
                result = new ParsedUserType(td.getName());
                in.pushType(tag);
// sampleRefs are sent as primitive types, see above
//        } else if(tag == Constant.SAMPLE) {
//                result = new SampleRefType();       
        } else {
                result = new PrimitiveType(tag);
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
