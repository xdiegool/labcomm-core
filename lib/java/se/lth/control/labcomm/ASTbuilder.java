package se.lth.control.labcomm;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.EOFException;

import se.lth.control.labcomm.TypeDef;
import se.lth.control.labcomm.TypeDefParser;

import se.lth.control.labcomm2014.compiler.LabComm;
import se.lth.control.labcomm2014.compiler.LabCommParser;

import se.lth.control.labcomm2014.compiler.List;
import se.lth.control.labcomm2014.compiler.Program;
import se.lth.control.labcomm2014.compiler.Decl;
import se.lth.control.labcomm2014.compiler.TypeDecl;
import se.lth.control.labcomm2014.compiler.SampleDecl;
import se.lth.control.labcomm2014.compiler.Type;
import se.lth.control.labcomm2014.compiler.VoidType;
import se.lth.control.labcomm2014.compiler.PrimType;
import se.lth.control.labcomm2014.compiler.UserType;
import se.lth.control.labcomm2014.compiler.StructType;
import se.lth.control.labcomm2014.compiler.Field;
import se.lth.control.labcomm2014.compiler.ArrayType;
import se.lth.control.labcomm2014.compiler.VariableArrayType;
import se.lth.control.labcomm2014.compiler.FixedArrayType;
import se.lth.control.labcomm2014.compiler.Dim;
import se.lth.control.labcomm2014.compiler.Exp;
import se.lth.control.labcomm2014.compiler.IntegerLiteral;
import se.lth.control.labcomm2014.compiler.VariableSize;


/** A class for building a JastAdd AST from the parsed types
 *  created by a TypeDefParser. This class depends on the LabComm compiler.
 */
public class ASTbuilder implements TypeDefParser.ParsedSymbolVisitor {

    private LinkedList<Type> typeStack;
    private LinkedList<Field> fieldStack;
    private TypeDefParser tdp;

    public ASTbuilder(TypeDefParser tdp) {
        this.typeStack = new LinkedList<Type>();
        this.fieldStack = new LinkedList<Field>();
        this.tdp = tdp;
    }

    private void assertStacksEmpty() throws RuntimeException {
        if(!typeStack.isEmpty()) {
           throw new RuntimeException("Error: type stack not empty"); 
        }
        if(!fieldStack.isEmpty()) {
           throw new RuntimeException("Error: field stack not empty"); 
        }
    }

    public void visit(TypeDefParser.TypeSymbol s){
        throw new Error("not implemented? needed?");

    }
    public void visit(TypeDefParser.SampleSymbol s){
        throw new Error("not implemented? needed?");

    }
    public void visit(TypeDefParser.NameSymbol s){
        throw new Error("not implemented? needed?");
    }

    public void visit(TypeDefParser.PrimitiveType t){
        typeStack.push(new PrimType(t.getName(), t.getTag()));
    }

// SampleRefs are currently represented as primitive types,
// see comment in TypeDefParser
//        public void visit(TypeDefParser.SampleRefType t){
//            typeStack.push(new SampleRefType());
//        }

    public void visit(TypeDefParser.ParsedStructType t){
        if(t.isVoid()) {
            typeStack.push(new VoidType());
        } else {
            List<Field> tmpF = new List<Field>();
            for( TypeDefParser.ParsedField f : t.getFields()) {
                f.accept(this);
                tmpF.add(fieldStack.pop());
            }
            typeStack.push(new StructType(tmpF));
        }
    }
    public void visit(TypeDefParser.ParsedField t){
        t.getType().accept(this);
        fieldStack.push(new Field(typeStack.pop(),t.getName()));
  
    }
    public void visit(TypeDefParser.ArrayType t){
        boolean isFixed = true;
        List<Exp> dim = new List<Exp>();
        for(int i : t.getIdx()) {
            if(i == 0) {
                dim.add(new VariableSize());
                isFixed = false;
            } else {
                dim.add(new IntegerLiteral(Integer.toString(i)));
            }
        }
        t.getType().accept(this);
        if(isFixed) {
            typeStack.push(new FixedArrayType(typeStack.pop(), dim));
        } else {
            typeStack.push(new VariableArrayType(typeStack.pop(), dim));
        }
  
    }
    public void visit(TypeDefParser.ParsedUserType t){
        typeStack.push(new UserType(t.getName()));
    }
  

    public Decl makeDecl(TypeDefParser.ParsedTypeDef d) {
        d.getType().accept(this);
        Decl result = new TypeDecl(typeStack.pop(), d.getName());
        return result;
    }

    private Program createAndCheckProgram(List<Decl> ds) {
       Program p = new Program(ds);
       LinkedList errors = new LinkedList();
       p.errorCheck(errors);
       if(errors.isEmpty()) {
           return p;
       } else {
           // This should not happen
           // get errors and throw exception
           StringBuilder sb = new StringBuilder();
           for (Iterator iter = errors.iterator(); iter.hasNext(); ) {
               String s = (String)iter.next();
               sb.append(s);
           }
           throw new RuntimeException("Internal error: parsed labcomm declaration has errors: "+sb.toString());
       }
    }

    /** Create a labcomm AST for the ParsedTypeDef d.
     *  If d is a sampleDecl, include the typedefs it depends on.
     */
    public Program makeProgram(TypeDefParser.ParsedTypeDef d) {
        List<Decl> ds = new List<Decl>();
        return makeProgram(d, ds);
    }

    /** Create a labcomm AST for the ParsedTypeDef d, including
     *  all declarations from p.
     *
     *
     *  If p is not null, and the source of d matches the 
     *  TypeDefParser this ASTbuilder was created for, 
     *  creates a new AST containing the declarations in p
     *  and d and (if d is a sampleDecl) its dependencies.
     *
     *  Otherwise, makeProgram(d)
     */
    public Program makeProgram(TypeDefParser.ParsedTypeDef d,
                               Program p) {
        if(p != null && d.checkSource(tdp)) {
           return makeProgram(d, p.getDecls().fullCopy());
        } else {
           return makeProgram(d);
        }
    }



    //TODO: The above method, adding to an arbitrary Program
    //      does not really allow "recreating the labcomm file"
    //      This method is a placeholder for 
    public Program makeProgram(TypeDefParser.ParsedTypeDef d,
                               TypeDefParser source) {
        throw new Error("Not yet implemented");
    }
    
    private void tryAdd(Decl decl, List<Decl> ds){
        boolean exists = false;

        // Check if decl already exists in ds
        // otherwise add it

        // Maybe add a Program.lookupDecl(String Name) 
        // to the NameAnalysis aspect instead of doing
        // it here.
        Iterator<Decl> it = ds.iterator();
        while(!exists && it.hasNext()) {
            Decl inList = it.next();
            if(inList.getName().equals(decl.getName())){
                exists = true;
            }
        }
        if(!exists) {
            ds.add(decl);
        } 
    }
    
    protected Program makeProgram(TypeDefParser.ParsedTypeDef d,
                                  List<Decl> ds) {
        assertStacksEmpty();

        // add dependencies for sample decls
        if(d.isSampleDef()) {
            Iterator<TypeDefParser.ParsedTypeDef> it = d.getDepIterator();
            while(it.hasNext()){
                Decl decl = makeDecl(it.next());
                tryAdd(decl,ds);
            }
        }
        ds.add(makeDecl(d));
        assertStacksEmpty();
        return createAndCheckProgram(ds);
    }
}
