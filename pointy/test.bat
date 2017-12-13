@echo OFF
SET directory=%1
SET iterations=%2
SET cs=pointy -in
SET ce=-out test.txt

@echo Doing %iterations% iterations for each file in ..\data\%directory%

FOR /R %%F IN (..\data\%directory\*) DO (
    FOR /L %%G IN (1, 1, %iterations%) DO (
        @echo simulated annealing
        %cs% ..\data\%directory%\%%~NXF %ce% -s >> .\tests\sim\%directory%\%%~NXF.txt
        IF EXIST test.txt del test.txt
    )
)


@echo Done
