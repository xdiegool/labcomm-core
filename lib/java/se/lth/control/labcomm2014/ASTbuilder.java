package se.lth.control.labcomm2014;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.EOFException;

import se.lth.control.labcomm2014.compiler.LabComm;
import se.lth.control.labcomm2014.compiler.LabCommParser;

import se.lth.control.labcomm2014.compiler.List;
import se.lth.control.labcomm2014.compiler.Specification;
import se.lth.control.labcomm2014.compiler.Decl;
import se.lth.control.labcomm2014.compiler.TypeDecl;
import se.lth.control.labcomm2014.compiler.SampleDecl;
import se.lth.control.labcomm2014.compiler.TypeInstance;
import se.lth.control.labcomm2014.compiler.DataType;
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
public class ASTbuilder implements SignatureSymbolVisitor {

        private LinkedList<DataType> typeStack;
        private LinkedList<Field> fieldStack;

        public ASTbuilder() {
            this.typeStack = new LinkedList<DataType>();
            this.fieldStack = new LinkedList<Field>();
        }

        private void assertStacksEmpty() throws RuntimeException {
            if(!typeStack.isEmpty()) {
               throw new RuntimeException("Error: type stack not empty");
            }
            if(!fieldStack.isEmpty()) {
               throw new RuntimeException("Error: field stack not empty");
            }
        }

        public void visit(TypeSymbol s){
            throw new Error("not implemented? needed?");

        }
        public void visit(SampleSymbol s){
            throw new Error("not implemented? needed?");

        }
        public void visit(NameSymbol s){
            throw new Error("not implemented? needed?");
        }

        public void visit(SigPrimitiveType t){
            typeStack.push(new PrimType(t.getName(), t.getTag()));
        }

// SampleRefs are currently represented as primitive types,
// see comment in TypeDefParser
//        public void visit(TypeDefParser.SampleRefType t){
//            typeStack.push(new SampleRefType());
//        }

        public void visit(SigStructType t){
            if(t.isVoid()) {
                typeStack.push(new VoidType());
            } else {
                List<Field> tmpF = new List<Field>();
                for( SigField f : t.getFields()) {
                    f.accept(this);
                    tmpF.add(fieldStack.pop());
                }
                typeStack.push(new StructType(tmpF));
            }
        }
        public void visit(SigField t){
            t.getType().accept(this);
            fieldStack.push(new Field(new TypeInstance(typeStack.pop(),t.getName())));

        }
        public void visit(SigArrayType t){
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
                typeStack.push(new FixedArrayType(typeStack.pop(), new Dim(dim)));
            } else {
                typeStack.push(new VariableArrayType(typeStack.pop(), new Dim(dim)));
            }

        }
        public void visit(SigUserType t){
            typeStack.push(new UserType(t.getName()));
        }


       public Decl makeDecl(SigTypeDef d) {
           d.getType().accept(this);
           Decl result = new TypeDecl(new TypeInstance(typeStack.pop(), d.getName()));
           return result;
       }

       private Specification createAndCheckSpecification(List<Decl> ds) {
            Specification p = new Specification(ds);
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

       public Specification makeSpecification(SigTypeDef d) {
           assertStacksEmpty();
           List<Decl> ds = new List<Decl>();

           ds.add(makeDecl(d));
           assertStacksEmpty();
           return createAndCheckSpecification(ds);
       }

       public Decl makeDecl(SigSampleDef d) {
           d.getType().accept(this);
           Decl result = new SampleDecl(new TypeInstance(typeStack.pop(), d.getName()));
           return result;
       }

       private void addAllDeps(List<Decl> ds, SigTypeDef d) {
           Iterator<SigTypeDef> it = d.getDepIterator();
           while(it.hasNext()){
               SigTypeDef dd = it.next();
               addAllDeps(ds,dd);
               ds.add(makeDecl(dd));
           }
       }

       public Specification makeSpecification(ParsedSampleDef d) {
           assertStacksEmpty();
           List<Decl> ds = new List<Decl>();

           addAllDeps(ds, d);

           ds.add(makeDecl(d));

           assertStacksEmpty();
           return createAndCheckSpecification(ds);
       }
    }

