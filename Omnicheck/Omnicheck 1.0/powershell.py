import subprocess  # IMPORT FOR SUB PROCESS . RUN METHOD

POWERSHELL_PATH = "powershell.exe"  # POWERSHELL EXE PATH
ps_script_path = "C:/Omni_Digital/Omni1/runasAdmin.ps1"  # YOUR POWERSHELL FILE PATH


class Utility:  # SHARED CLASS TO USE IN OUR PROJECT

    @staticmethod    # STATIC METHOD DEFINITION
    def run_ftp_upload_powershell_script(script_path, *params):  # SCRIPT PATH = POWERSHELL SCRIPT PATH,  PARAM = POWERSHELL SCRIPT PARAMETERS ( IF ANY )

        commandline_options = [POWERSHELL_PATH, '-ExecutionPolicy', 'Unrestricted', script_path]  # ADD POWERSHELL EXE AND EXECUTION POLICY TO COMMAND VARIABLE
        for param in params:  # LOOP FOR EACH PARAMETER FROM ARRAY
            commandline_options.append("'" + param + "'")  # APPEND YOUR FOR POWERSHELL SCRIPT

        process_result = subprocess.run(commandline_options, stdout = subprocess.PIPE, stderr = subprocess.PIPE, universal_newlines = True)  # CALL PROCESS

        loop_iter = 0
        while (process_result.stderr):
            if (loop_iter == 3):
                break
            process_result = subprocess.run(commandline_options, stdout = subprocess.PIPE, stderr = subprocess.PIPE, universal_newlines = True)  # CALL PROCESS
            loop_iter += 1
            print(loop_iter)

        print(process_result.returncode)    # PRINT RETURN CODE OF PROCESS  0 = SUCCESS, NON-ZERO = FAIL  
        print(process_result.stderr)    # PRINT STANDARD ERROR FROM POWERSHELL ( IF ANY OTHERWISE ITS NULL|NONE )

        if process_result.returncode == 0:  # COMPARING RESULT
            Message = "Success !"
        else:
            Message = "Error Occurred !"

        return Message  # RETURN MESSAGE

def main():
    Utility.run_ftp_upload_powershell_script(POWERSHELL_PATH, ps_script_path)

if __name__ == "__main__":
    main()