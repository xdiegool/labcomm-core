package se.lth.control.labcomm2014;

public class ParsedTypeDef extends SigTypeDef{
    private int idx;

    ParsedTypeDef(int idx, String name){
        super(name);
        this.idx = idx;
    }

    ParsedTypeDef(int idx, String name, DataType type) {
        super(name, type);
        this.idx = idx;
    }

    ParsedTypeDef(ParsedTypeDef td) {
        super(td);
        this.idx = td.getIndex();
    }

    int getIndex() {
        return idx;
    }
}


