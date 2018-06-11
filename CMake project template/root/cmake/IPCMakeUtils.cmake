macro(setTargetFolder TARGETNAME FOLDERNAME)
    set_target_properties(${TARGETNAME}
        PROPERTIES
            FOLDER ${FOLDERNAME})
endmacro()