#############################################################################
##
#W  error.g                    GAP library                 Steve Linton
##
#H  @(#)$Id: error.g,v 4.9 2009/01/26 16:40:29 alexk Exp $
##
#Y  Copyright (C) 2007 The GAP Group
##
##  Error handling, break loop, etc. Now in GAP
##
Revision.error_g :=
    "@(#)$Id: error.g,v 4.9 2009/01/26 16:40:29 alexk Exp $";

errorCount := 0;
ErrorLevel := 0;
MakeReadWriteGlobal("OnQuit");
OnQuit := fail;
QUITTING := false;
BreakOnError := true;

BIND_GLOBAL("AncestorLVars", function(lv, depth)
    local i;
    for i in [1..depth] do
        lv := ParentLVars(lv);
    od;
    return lv;
end);

ErrorLVars := fail;




BIND_GLOBAL("WHERE", function( context, depth)
    local   bottom;
    if depth <= 0 then
        return;
    fi;
    bottom := GetBottomLVars();
    while depth > 0  and context <> bottom do
        PRINT_CURRENT_STATEMENT(context);
        Print(" called from\n");
        context := ParentLVars(context);
        depth := depth-1;
    od;
    if depth = 0 then 
        Print("...\n");
    else
        Print("<function>( <arguments> ) called from read-eval loop\n");
    fi;
end);


BIND_GLOBAL("Where", function(arg)
    local   depth;
    if LEN_LIST(arg) = 0 then
        depth := 5;
    else
        depth := arg[1];
    fi;
    
    if ErrorLVars = fail or ErrorLVars = GetBottomLVars() then
        Print("not in any function\n");
    else
        WHERE(ParentLVars(ErrorLVars),depth);
    fi;
end);

OnBreak := Where;

#OnBreak := function() 
#    if IsLVarsBag(ErrorLVars) then
#        if ErrorLVars <> BottomLVars then
#            WHERE(ParentLVars(ErrorLVars),5); 
#        else
#            Print("<function><argume
#    else
#        WHERE(ParentLVars(GetCurrentLVars()),5);
#   fi;
#end;

BIND_GLOBAL("ErrorCount", function()
    return errorCount;
end);


#
# ErrorInner(context, justQuit, mayReturnVoid, mayReturnObj, lateMessage, .....)
# 
#

Unbind(ErrorInner);

BIND_GLOBAL("ErrorInner",
        function( arg )
    local   context, mayReturnVoid,  mayReturnObj,  lateMessage,  earlyMessage,  
            x,  prompt,  res, errorLVars, justQuit, printThisStatement;

	context := arg[1].context;
    if not IsLVarsBag(context) then
        PrintTo("*errout*", "ErrorInner:   option context must be a local variables bag\n");
        LEAVE_ALL_NAMESPACES();
        JUMP_TO_CATCH(1);
    fi; 
        
    if IsBound(arg[1].justQuit) then
        justQuit := arg[1].justQuit;
        if not justQuit in [false, true] then
            PrintTo("*errout*", "ErrorInner: option justQuit must be true or false\n");
            LEAVE_ALL_NAMESPACES();
            JUMP_TO_CATCH(1);
        fi;
    else
        justQuit := false;
    fi;
        
    if IsBound(arg[1].mayReturnVoid) then
        mayReturnVoid := arg[1].mayReturnVoid;
        if not mayReturnVoid in [false, true] then
            PrintTo("*errout*", "ErrorInner: option mayReturnVoid must be true or false\n");
            LEAVE_ALL_NAMESPACES();
            JUMP_TO_CATCH(1);
        fi;
    else
        mayReturnVoid := false;
    fi;
        
    if IsBound(arg[1].mayReturnObj) then
        mayReturnObj := arg[1].mayReturnObj;
        if not mayReturnObj in [false, true] then
            PrintTo("*errout*", "ErrorInner: option mayReturnObj must be true or false\n");
            LEAVE_ALL_NAMESPACES();
            JUMP_TO_CATCH(1);
        fi;
    else
        mayReturnObj := false;
    fi;
     
    if IsBound(arg[1].printThisStatement) then
        printThisStatement := arg[1].printThisStatement;
        if not printThisStatement in [false, true] then
            PrintTo("*errout*", "ErrorInner: option printThisStatement must be true or false\n");
            LEAVE_ALL_NAMESPACES();
            JUMP_TO_CATCH(1);
        fi;
    else
        printThisStatement := true;
    fi;
        
    if IsBound(arg[1].lateMessage) then
        lateMessage := arg[1].lateMessage;
        if not lateMessage in [false, true] and not IsString(lateMessage) then
            PrintTo("*errout*", "ErrorInner: option lateMessage must be a string or false\n");
            LEAVE_ALL_NAMESPACES();
            JUMP_TO_CATCH(1);
        fi;
    else
        lateMessage := "";
    fi;
        
    earlyMessage := arg[2];
    if Length(arg) <> 2 then
        PrintTo("*errout*","ErrorInner: new format takes exactly two arguments\n");
        LEAVE_ALL_NAMESPACES();
        JUMP_TO_CATCH(1);
    fi;
        
    ErrorLevel := ErrorLevel+1;
    errorCount := errorCount+1;
    errorLVars := ErrorLVars;
    ErrorLVars := context;
    if QUITTING or not BreakOnError then
        PrintTo("*errout*","Error, ");
        for x in earlyMessage do
            PrintTo("*errout*",x);
        od;
        PrintTo("*errout*","\n");
        ErrorLevel := ErrorLevel-1;
        ErrorLVars := errorLVars;
        if ErrorLevel = 0 then LEAVE_ALL_NAMESPACES(); fi;
        JUMP_TO_CATCH(0);
    fi;
    PrintTo("*errout*","Error, ");
    for x in earlyMessage do
        PrintTo("*errout*",x);
    od;
    if printThisStatement then 
        if context <> GetBottomLVars() then
            PrintTo("*errout*"," in\n  \c");
            PRINT_CURRENT_STATEMENT(context);
            Print("\c");
            PrintTo("*errout*"," called from \n");
        else
            PrintTo("*errout*","\c\n");
        fi;
    else
        PrintTo("*errout*"," called from\c\n");
    fi;
    if IsBound(OnBreak) and IsFunction(OnBreak) then
        OnBreak();
    fi;
    if IsString(lateMessage) then
        PrintTo("*errout*",lateMessage,"\n");
    elif lateMessage then
        if IsBound(OnBreakMessage) and IsFunction(OnBreakMessage) then
            OnBreakMessage();
        fi;
    fi;
    if ErrorLevel > 1 then
        prompt := Concatenation("brk_",String(ErrorLevel),"> ");
    else
        prompt := "brk> ";
    fi;
    if not justQuit then
        res := SHELL(context,mayReturnVoid,mayReturnObj,1,false,prompt,false,"*errin*","*errout*",false);
    else
        res := fail;
    fi;
    ErrorLevel := ErrorLevel-1;
    ErrorLVars := errorLVars;
    if res = fail then
        if IsBound(OnQuit) and IsFunction(OnQuit) then
            OnQuit();
        fi;
	if ErrorLevel = 0 then LEAVE_ALL_NAMESPACES(); fi;
        if not justQuit then
	   # dont try and do anything else after this before the longjump 	
            SetUserHasQuit(1);	
        fi;
        JUMP_TO_CATCH(3);
    fi;
    if Length(res) > 0 then
        return res[1];
    else
        return;
    fi;
end);

Unbind(Error);

BIND_GLOBAL("Error",
        function(arg)
    ErrorInner(rec( context := ParentLVars(GetCurrentLVars()),
                               mayReturnVoid := true,
                               lateMessage := true,
                               printThisStatement := false),
                               arg);
end);

ADD_LIST(POST_RESTORE_FUNCS, 
        function()
    errorCount := 0;
    ErrorLevel := 0;
    QUITTING := false;
end);