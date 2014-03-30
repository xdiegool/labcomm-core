MODULE Follow
    PROC LCjt2jt(VAR LCRobot_jointtarget a, VAR jointtarget b)
        b.robax.rax_1 := a.robax.rax_1;
        b.robax.rax_2 := a.robax.rax_2;
        b.robax.rax_3 := a.robax.rax_3;
        b.robax.rax_4 := a.robax.rax_4;
        b.robax.rax_5 := a.robax.rax_5;
        b.robax.rax_6 := a.robax.rax_6;
        
        b.extax.eax_a := a.extax.eax_a;
        b.extax.eax_b := a.extax.eax_b;
        b.extax.eax_c := a.extax.eax_c;
        b.extax.eax_d := a.extax.eax_d;
        b.extax.eax_e := a.extax.eax_e;
        b.extax.eax_f := a.extax.eax_f;
    ENDPROC
    
    PROC handle_jointtarget(LCRobot_jointtarget val)
        VAR jointtarget jt;
        LCjt2jt val, jt;
        MoveAbsJ jt, v100, z1, tool0, \WObj:=wobj0;
    ENDPROC
    
    PROC main()
        VAR LabComm_Stream st;
        VAR socketdev srv;
        SocketCreate srv;
        SocketBind srv, "127.0.0.1", 51337;
        SocketListen srv;
        WHILE TRUE DO
            SocketAccept srv, st.soc;
            Receive_targets st;
        ENDWHILE
    ENDPROC
    
    PROC Receive_targets(VAR LabComm_Stream st)
        VAR Decoder d;
        VAR LabComm_Decoder_Sample s{1};
        Init_Decoder d, st;
        % "LCRobot:Dec_Reg_jointtarget" % s{1}, "handle_jointtarget";
        WHILE TRUE DO
            Decode_One d, st, s;
        ENDWHILE
    ERROR
        SocketClose st.soc;
        RETURN;
    ENDPROC
    
ENDMODULE