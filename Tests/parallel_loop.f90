      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        INTEGER :: errors = 0

        !Initilization
        a = 0
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(b)
        CALL RANDOM_NUMBER(c)

        !$acc data copy(a(1:LOOPCOUNT))
          !$acc parallel loop
          DO x = 1, LOOPCOUNT
            a(x) = 1
          END DO
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (abs(a(x) - 1) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        !$acc data copy(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              a(x) = b(x)
            END DO
            !$acc loop
            DO x = 1, LOOPCOUNT
              a(x) = a(x) + c(x)
            END DO
          !$acc end parallel
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (abs(a(x) - (b(x) + c(x))) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        test = errors
      END


      PROGRAM test_kernels_async_main
      IMPLICIT NONE
      INTEGER :: failed, success !Number of failed/succeeded tests
      INTEGER :: num_tests,crosschecked, crossfailed, j
      INTEGER :: temp,temp1
      INCLUDE "acc_testsuite.fh"
      INTEGER test


      CHARACTER*50:: logfilename !Pointer to logfile
      INTEGER :: result

      num_tests = 0
      crosschecked = 0
      crossfailed = 0
      result = 1
      failed = 0

      !Open a new logfile or overwrite the existing one.
      logfilename = "OpenACC_testsuite.log"
!      WRITE (*,*) "Enter logFilename:"
!      READ  (*,*) logfilename

      OPEN (1, FILE = logfilename)

      WRITE (*,*) "######## OpenACC Validation Suite V 2.5 ######"
      WRITE (*,*) "## Repetitions:", N
      WRITE (*,*) "## Loop Count :", LOOPCOUNT
      WRITE (*,*) "##############################################"
      WRITE (*,*)

      WRITE (*,*) "--------------------------------------------------"
      WRITE (*,*) "Test of parallel_loop"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of parallel_loop"
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*)
      WRITE (1,*) "(Crosstests should fail)"
      WRITE (1,*)

      DO j = 1, N
        temp =  test()
        IF (temp .EQ. 0) THEN
          WRITE (1,*)  j, ". test successfull."
          success = success + 1
        ELSE
          WRITE (1,*) "Error: ",j, ". test failed."
          failed = failed + 1
        ENDIF
      END DO


      IF (failed .EQ. 0) THEN
        WRITE (1,*) "Directive worked without errors."
        WRITE (*,*) "Directive worked without errors."
        result = 0
        WRITE (*,*) "Result:",result
      ELSE
        WRITE (1,*) "Directive failed the test ", failed, " times."
        WRITE (*,*) "Directive failed the test ", failed, " times."
        result = failed * 100 / N
        WRITE (*,*) "Result:",result
      ENDIF
      CALL EXIT (result)
      END PROGRAM


