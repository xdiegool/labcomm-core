package se.lth.control.labcomm2014;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.EOFException;

import se.lth.control.labcomm2014.Decoder;
import se.lth.control.labcomm2014.DecoderChannel;
import se.lth.control.labcomm2014.TypeDef;
import se.lth.control.labcomm2014.TypeBinding;

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
        void onTypeDef(SigTypeDef d);
    }

    private HashMap<Integer,TypeDef> typeDefs;
    private HashMap<Integer,Integer> typeBindings;
    private HashSet<TypeDefListener> listeners;
    private LinkedList<ParsedSampleDef> sampleDefs;
    private HashMap<Integer,ParsedTypeDef> pts;
    private Decoder decoder;

    protected TypeDefParser(Decoder d) {
        this.decoder = d;
        typeDefs = new HashMap<Integer,TypeDef>();
        typeBindings = new HashMap<Integer,Integer>();
        listeners = new HashSet<TypeDefListener>();
        sampleDefs = new LinkedList<ParsedSampleDef>();
        pts = new HashMap<Integer,ParsedTypeDef>();
    }

    public void addListener(TypeDefListener l) {
        listeners.add(l);
    }


    public Iterator<ParsedSampleDef> sampleDefIterator() {
        return sampleDefs.iterator();
    }

    public void handle_TypeDef(TypeDef d) throws java.io.IOException {
        System.out.println("handle_TypeDef: "+d.getIndex());
        typeDefs.put(d.getIndex(), d);
        ParsedTypeDef td = parseSignatureTD(d);
        pts.put(d.getIndex(), td);
    }

    public void handle_TypeBinding(TypeBinding d) throws java.io.IOException {
        TypeDef td;
        if(d.isSelfBinding()){
             td = new SelfBinding(d.getSampleIndex(), decoder);
        } else {
            typeBindings.put(d.getSampleIndex(), d.getTypeIndex());
            td = getTypeDefForIndex(d.getSampleIndex());
        }
        System.out.println("handle_TypeBinding: "+d.getSampleIndex());
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
            for(SigTypeDef dep : ((ParsedSampleDef)d).getDependencies()) {
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

    public LinkedList<SignatureSymbol> symbolify() {

        LinkedList<SignatureSymbol> result = new LinkedList<SignatureSymbol>();

        Iterator<ParsedSampleDef> sdi = sampleDefIterator();

        while(sdi.hasNext()) {
            SigTypeDef sd = sdi.next();
            result.add(new SampleSymbol());
            result.add(sd.getType());
            result.add(new NameSymbol(sd.getName()));

            Iterator<SigTypeDef> di = sd.getDepIterator();
            while(di.hasNext()) {
                SigTypeDef d = di.next();
                result.add(new TypeSymbol());
                result.add(d.getType());
                result.add(new NameSymbol(d.getName()));
            }
        }
        return result;
    }

    public String symbolString() {
        Iterator<SignatureSymbol> i = symbolify().iterator();

        StringBuilder sb = new StringBuilder();

        while(i.hasNext()) {
            sb.append(i.next().toString());
        }
        return sb.toString();
    }

// SampleRefType currently not sent, se above
//    public class SampleRefType extends DataType {
//        public void accept(SignatureSymbolVisitor v) {
//            v.visit(this);
//        }
//
//        public String toString() {
//            return "sample";}
//    }

    private class ParserState {
        private ByteArrayInputStream bis;
        private DataInputStream in;
        private TypeDef current;
        private ParsedTypeDef currentParsed;

        private LinkedList<TypeDef> typeStack;

        ParsedTypeDef newTypeDef() {
            currentParsed =new ParsedTypeDef(getCurrentIndex(), getCurrentName());
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

        void addTypeUse(int tag) {
            SigTypeDef td = pts.get(tag);
            if(td != null) {
                currentParsed.addDependency(td);
            } else {
                System.out.println("******* WARNING: TypeDefParser:addTypeUse ("+tag+"): null???");
            }
        }
        /** return name, (if any, or "") for now */
        String decodeIntentions() throws IOException {
            int n = decodePacked32() & 0xffffffff;
            if(n==0) return "";

            String name = "";

            for(int i=0; i<n;i++) {
                int klen = decodePacked32() & 0xffffffff;
                byte[] kchars = new byte[klen];
                for(int k=0; k<klen; k++) {
                    kchars[k] = in.readByte();
                }

                int vlen = decodePacked32() & 0xffffffff;
                byte[] vchars = new byte[vlen];
                for(int j=0; j<vlen; j++) {
                    vchars[j] = in.readByte();
                }
                if(klen==0) {
                  name = new String(vchars);
                }
            }
            return name;
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

        void skipBytes(int len) throws IOException {
            for(int i=0; i<len; i++) {
                in.readByte();
            }
        }
    }

    private ParsedTypeDef parseSignatureTD(TypeDef td) throws IOException{
        return parseSignatureTD(td, new ParserState(td));
    }

    private ParsedTypeDef parseSignatureTD(TypeDef td, ParserState s) throws IOException{

        ParsedTypeDef result=null;
        s.popType();
        result = parseTypeDef(s);
        return result;
    }

    public ParsedSampleDef parseSignature(TypeDef td) throws IOException{
        ParserState s = new ParserState(td);

        ParsedSampleDef result=null;
        try {
            result = new ParsedSampleDef(parseSignatureTD(td,s));

//            while(s.moreTypes()) {
//                s.popType();
//                result.addDependency(parseTypeDef(s));
//            }
        } catch(java.io.EOFException ex) {
            System.out.println("EOF: self_binding");
        }
        return result;
    }

    private SigArrayType  parseArray(ParserState in) throws IOException {
        int numIdx = in.decodePacked32();
        int idx[] = new int[numIdx];
        for(int i=0; i<numIdx; i++){
            idx[i] = in.decodePacked32();
        }
        int type = in.decodePacked32();
        DataType elementType = lookupType(type, in);
        SigArrayType result = new SigArrayType(elementType, idx);
        for(int i=0; i<numIdx; i++){
            idx[i] = in.decodePacked32();
        }
        return result;
    }

    private SigStructType parseStruct(ParserState in) throws IOException {
        int numSigFields = in.decodePacked32();
        SigStructType result = new SigStructType(numSigFields);
        for(int i=0; i<numSigFields; i++) {
            result.addField(parseSigField(in));
        }
        return result;
    }

    private SigField parseSigField(ParserState in) throws IOException {
        String name = in.decodeIntentions();
        return new SigField(name, parseType(in, false));
    }

    private DataType lookupType(int tag, ParserState in) {
        DataType result;
        if(tag >= Constant.FIRST_USER_INDEX) {
                TypeDef td = typeDefs.get(tag);
                result = new SigUserType(td.getName());
                in.addTypeUse(tag);
                in.pushType(tag);
// sampleRefs are sent as primitive types, see above
//        } else if(tag == Constant.SAMPLE) {
//                result = new SampleRefType();
        } else {
                result = new SigPrimitiveType(tag);
        }
        return result;
    }

    private ParsedSampleDef parseSampleTypeDef(ParserState in) throws IOException {
        ParsedTypeDef td = parseTypeDef(in, true);
        return new ParsedSampleDef(td);
    }
    private ParsedTypeDef parseTypeDef(ParserState in) throws IOException {
        return parseTypeDef(in, false);
    }

    private void addParsedTypeDef(ParsedTypeDef td) {
        int idx = td.getIndex();
        if(idx>=0x40) {
            pts.put(idx, td);
        }
    }

    private ParsedTypeDef parseTypeDef(ParserState in, boolean parseIntentions) throws IOException {
        ParsedTypeDef result = in.newTypeDef();
        result.setType(parseType(in, false));
        addParsedTypeDef(result);
        return result;
    }

    private DataType parseType(ParserState in, boolean parseIntentions) throws IOException {
        if(parseIntentions) {
            String intentions = in.decodeIntentions();
            if(intentions.length()>0) {
                System.out.println("parseType intentions ("+intentions);
            } else {
                System.out.println("no intentions");
            }
        } else {
//            System.out.println("not parsing intentions");
        }

        int tag = in.decodePacked32();
        DataType result = null;
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
