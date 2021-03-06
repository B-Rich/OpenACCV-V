      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        INTEGER :: errors = 0
        LOGICAL,DIMENSION(1):: has_device

        has_device(1) = .TRUE.
        !$acc enter data copyin(has_device(1:1))
        !$acc parallel present(has_device(1:1))
          has_device(1) = .FALSE.
        !$acc end parallel

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
        !$acc data copyout(c(1:LOOPCOUNT)) present(a(1:LOOPCOUNT), &
           !$acc b(1:LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
        !$acc end data
        !$acc exit data delete(a(:LOOPCOUNT), b(:LOOPCOUNT))
        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (has_device(1) .eqv. .TRUE.) THEN
          CALL RANDOM_NUMBER(a)
          CALL RANDOM_NUMBER(b)
          c = 0

          !$acc enter data copyin(c(1:LOOPCOUNT))
          !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) &
             !$acc present(c(1:LOOPCOUNT))
            !$acc parallel
              !$acc loop
              DO x = 1, LOOPCOUNT
                c(x) = a(x) + b(x)
              END DO
            !$acc end parallel
          !$acc end data
          !$acc exit data delete(c(:LOOPCOUNT))

          DO x = 1, LOOPCOUNT
            IF (abs(c(x)) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
        END IF
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
      WRITE (*,*) "Test of exit_data_delete_no_lower_bound"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of exit_data_delete_no_lower_bound"
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


