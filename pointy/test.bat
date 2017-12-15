@echo OFF

SET directory=%1
SET iterations=%2
SET graphic=%3
SET simulated=%4
SET testdir=data
SET topdir=hand_made

IF %5 == 1 (
    SET testdir=data\test
)

echo %directory% | findstr "real_world" > nul
IF NOT ERRORLEVEL 1  (
    SET topdir=real_world
) ELSE (
    echo %directory% | findstr "generated" > nul
    IF NOT ERRORLEVEL 1  (
        SET topdir=generated
    )
)

SET cs=pointy -in
SET ce=-out test.txt

@echo Compiling pointy

make clean
make

@echo Doing %iterations% iterations for each file in ..\%testdir%\%directory% (%topdir%)

FOR /R ..\%testdir%\%directory%\ %%F IN (*) DO (

    IF %simulated% == 1 (
        FOR /L %%G IN (1, 1, %iterations%) DO (
            %cs% ..\%testdir%\%directory%\%%~NXF %ce% -s >> .\tests\sim\%topdir%\%%~NF.txt
        )
        IF EXIST test.txt DEL test.txt
    )

    IF %graphic% == 1 (
        FOR /L %%G IN (1, 1, %iterations%) DO (
            %cs% ..\%testdir%\%directory%\%%~NXF %ce% -g >> .\tests\graphic\%topdir%\%%~NF.txt
        )
        IF EXIST test.txt DEL test.txt
    )
)

@echo Done
