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

// for BinaryScanner

import beaver.Scanner;
import beaver.Symbol;
import se.lth.control.labcomm2014.compiler.LabComm;
import se.lth.control.labcomm2014.compiler.LabCommParser;

import se.lth.control.labcomm2014.compiler.List;
import se.lth.control.labcomm2014.compiler.Program;
import se.lth.control.labcomm2014.compiler.Decl;
import se.lth.control.labcomm2014.compiler.TypeDecl;
import se.lth.control.labcomm2014.compiler.SampleDecl;
import se.lth.control.labcomm2014.compiler.Type;
//import se.lth.control.labcomm2014.compiler.VoidType;
//import se.lth.control.labcomm2014.compiler.SampleRefType;
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

////////////

public class TypeDefVisitor implements TypeDefParser.ParsedSymbolVisitor {

///// tree building
//
//
        private LinkedList<Type> typeStack;
        private LinkedList<Field> fieldStack;

        public TypeDefVisitor() {
            this.typeStack = new LinkedList<Type>();
            this.fieldStack = new LinkedList<Field>();
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
        public void visit(TypeDefParser.ParsedStructType t){
            List<Field> tmpF = new List<Field>();
            for( TypeDefParser.ParsedField f : t.getFields()) {
                f.accept(this);
                tmpF.add(fieldStack.pop());
            }
            typeStack.push(new StructType(tmpF));
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
       
       public Program makeProgram(TypeDefParser.ParsedTypeDef d) {
           List<Decl> ds = new List<Decl>();

           ds.add(makeDecl(d));
           return new Program(ds);
       }

       public Decl makeDecl(TypeDefParser.ParsedSampleDef d) {
           d.getType().accept(this);
           Decl result = new SampleDecl(typeStack.pop(), d.getName());
           return result;
       }
       public Program makeProgram(TypeDefParser.ParsedSampleDef d) {
           List<Decl> ds = new List<Decl>();

           Iterator<TypeDefParser.ParsedTypeDef> it = d.getDepIterator();
           while(it.hasNext()){
               ds.add(makeDecl(it.next()));
           }

           ds.add(makeDecl(d));

           return new Program(ds);
       }
    }

