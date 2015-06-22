package se.lth.control.labcomm2014;
    /* An interface for using Visitor pattern to traverse
     * the type tree
     */
    public interface SignatureSymbolVisitor {
        void visit(TypeSymbol s);
        void visit(SampleSymbol s);
        void visit(NameSymbol s);
        void visit(SigPrimitiveType t);
        //sampleRefs are sent as primitive types
        //Put this back if that is changed to SampleRefType
        //void visit(SampleRefType t);
        void visit(SigStructType t);
        void visit(SigField t);
        void visit(SigArrayType t);
        void visit(SigUserType t);
    }
