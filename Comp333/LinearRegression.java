//Jared Fowler
//Comp 333 Assignment 1  
//January 28, 2014
//Fortran to Java

/* Note - Original Fortran code will be written in commented out code, followed by its 
 *        translation into Java.  I will also add my own comments by using the double forward slashes //
 */

import java.util.Scanner;	//To read user input from the console


public class LinearRegression {
	
	
	static public void main(String[] args){
		
		/*
		 *C 	The Steve Sharpe (University of Bridgeport)
		 *C 			Irvin Levy (Gordon College) Linear Regression Package
		 *C 
		 *C			*  *  *  *  VARIABLE LIST  *  *  *  *
		 *C						S1 - Sum of X values
		 *C 					S2 - Sum of Y values
		 *C 					S3 - Sum of X * Y
		 *C 					S4 - Sum of X * X
		 *C 					S5 - Sum of Y * Y
		 *C 					D  - Sum of Y(obs) - Y(calc) squared
		 *C 					S  - Slope
		 *C 					B  - Y-intercept
		 *C 					E1 - Error in the slope
		 *C 					E2 - Error in the intercept
		 *C 					T  - Beta function for linear regression
		 *C 					R  - Correlation coefficient
		 *C 
		 *C 			Error function uses (N-2) weighting
		 *C 
		 *C 
		 */
		
		
		/*
		 * The Steve Sharpe (University of Bridgeport)
		 * 		Irvin Levy (Gordon College) Linear Regression Package
		 * 
		 * 		*  *  *  *  VARIABLE LIST  *  *  *  *
		 * 				S1 - Sum of X values
		 * 				S2 - Sum of Y values
		 * 				S3 - Sum of X * Y
		 * 				S4 - Sum of X * X
		 * 				S5 - Sum of Y * Y
		 * 				D  - Sum of Y(obs) - Y(calc) squared
		 * 				S  - Slope
		 * 				B  - Y-intercept
		 * 				E1 - Error in the slope
		 * 				E2 - Error in the intercept
		 * 				T  - Beta function for linear regression
		 * 				R  - Correlation coefficient
		 * 
		 * 		Error function uses (N-2) weighting
		 * 
		 * 
		 */
		
		
		/*
		 * 		DIMENSION X(52), Y(2,50), LITERL(2)
		 * 		DOUBLE PRECISION S1,S2,S3,S4,S5,T,S,B,D,R,E1,E2,BBAR
		 */
		
		//Arrays in Fortran start with index 1, so I will increase my array size by one to compensate
		//Also, in Fortran 77 the use of DIMENSION would mean an array of real numbers. I will use doubles
		double[] X = new double[53];
		double[][] Y = new double[3][51];
		char[] LITERL = new char[3];
		double S1,S2,S3,S4,S5,T,S,B,D,R,E1,E2,BBAR;
		
		
		
		
		
		/*
		 * 		WRITE (5,10)
		 */
		//Statements like this specify to write to device '5' what is located at position '10' in the code. We will ignore them.
		
		
		
		
		
		/*
		 *10	FORMAT('0',1X,'*   *   *  LINEAR REGRESSION ANALYSIS  *   *   *',//)
		 *		WRITE (5,20)
		 *20	FORMAT (1X,'HOW MANY PAIRS TO BE ANALYZED?'$)
		 * 		READ (5,*) N
		 */
		
		//The '1X' represents 1 horizontal space.  The '/' is a new line command.
		System.out.print(" *   *   *  LINEAR REGRESSION ANALYSIS  *   *   *\n\n");
		System.out.print(" HOW MANY PAIRS TO BE ANALYZED?");
		//Again, the '5' in Read(5,*) N , represents the output i/o device (the screen)
		//* represents the wild card, probably the user input in this case, which will be stored into N
		Scanner reader = new Scanner(System.in);
		int N = reader.nextInt();
		
		
		
		
		/*
		 * 		IF (N.GT.50) GOTO 70
		 * 		WRITE (5,30)
		 *30	FORMAT (//1X, 'Enter one pair at a time')
		 *		WRITE (5,40)
		 *40	FORMAT (1X, 'and separate X from Y with a comma.'//)
		 *		WRITE (5,50)
		 *50	FORMAT (1X, 'Enter pair number one : '$)
		 *		READ (5,*) X(1), Y(1,1)
		 *				DO 60 I=2,N
		 *				WRITE (5,55) I
		 *				FORMAT (1X, 'Enter pair number',I3,' : '$)
		 *				READ (5,*) X(I), Y(1,I)
		 *60			CONTINUE
		 *		GOTO 90
		 *70	WRITE (5,80)
		 *80	FORMAT(1X,'At present this program can only handle 50 data pairs.')
		 *		STOP
		 */
		
		//The "GOTO" acts like a label in assembly.  70 is not the code line, but a label.
		if(N > 50){
			System.out.println("At present this program can only handle 50 data pairs.");
			reader.close();
			return;	
		}
		System.out.print("\n\n Enter one pair at a time"
				+ " and separate X from Y with a comma.\n\n");
		System.out.print(" Enter pair number one : ");
		String[] tempStrArry = reader.next().split("[,]");
		X[1] = Double.valueOf(tempStrArry[0]);
		Y[1][1] = Double.valueOf(tempStrArry[1]);
		for(int I = 2; I <= N; I++){
			System.out.printf(" Enter pair number %3d : ",I);
			tempStrArry = reader.next().split("[,]");
			X[I] = Double.valueOf(tempStrArry[0]);
			Y[1][I] = Double.valueOf(tempStrArry[1]);
		}	
		
		
		
		
		/*
		 * 90	WRITE (5,100)
		 * 100	FORMAT(1X, 'Would you like to review the data?'$)
		 * 		READ (5,110) LITERAL(1)
		 * 110	FORMAT(A1)
		 * 		IF (LITERL(1).EQ.'N') GOTO 140
		 * 				DO 130 I=1,N
		 * 				WRITE (5,120) I, X(I), Y(1,I)
		 * 120 			FORMAT(1X, 'DATA PAIR',I3,').  ',2F20.10)
		 * 130			CONTINUE
		 */
		
		String tempStr;  //This is my own string for functionality use. Also, I use a do-while loop to satisfy the GOTO 90 statement later on
		do{
			System.out.print(" Would you like to review the data?");
			tempStr = new String(reader.next());
			LITERL[1] = tempStr.charAt(0);
			if(Character.toLowerCase(LITERL[1]) != 'n'){
				for(int I = 1; I <=N; I++){
					tempStr = new String(String.format(" DATA PAIR%3d).  %20.10f %20.10f", I, X[I], Y[1][I]));
					System.out.println(tempStr);
				}
			}
			
			
			
			
			
			/*
			 * 140	WRITE (5,150)
			 * 150	FORMAT(1X,'Would you like to make any changes?'$)
			 * 		READ(5,110) LITERL(1)
			 * 		IF (LITERL(1).EQ.'N') GOTO 200
			 * 		WRITE (5,160)
			 * 160	FORMAT(1X,'To change a data pair, enter the')
			 * 		WRITE (5,165)
			 * 165	FORMAT(1X,'Pair Number and the new X, Y pair.')
			 * 		WRITE(5,180)
			 * 180	FORMAT('0',1X,How many changes shall we make?'$)
			 * 		READ (5,*) ICHANG
			 * 				DO 190 I=1,ICHANG
			 * 				WRITE (5,185) I
			 * 185			FORMAT(1X, 'CHANGE NUMBER',I4,' : '$)
			 * 				READ (5,*) C1, C2, C3
			 * 				X(C1)=C2
			 * 				Y(1,C1)=C3
			 * 
			 * 190			CONTINUE
			 * 		GOTO 90
			 */
			
			System.out.print(" Would you like to make any changes?");
			tempStr = new String(reader.next());
			LITERL[1] = tempStr.charAt(0);
			if(Character.toLowerCase(LITERL[1]) != 'n'){
				System.out.println(" To change a data pair, enter the "
						+ "Pair Number and the new X, Y pair.");
				System.out.print(" How many changes shall we make?");
				int ICHANG = reader.nextInt();
				for(int I = 1; I <= ICHANG; I++){
					tempStr = new String(String.format(" CHANGE NUMBER%4d : ", I));
					System.out.print(tempStr);
					int C1 = reader.nextInt();
					tempStrArry = reader.next().split("[,]");
					double C2 = Double.valueOf(tempStrArry[0]);
					double C3 = Double.valueOf(tempStrArry[1]);
					X[C1] = C2;
					Y[1][C1] = C3;
				}
			}
			
		}while(Character.toLowerCase(LITERL[1]) != 'n');
		
		
		
		
		/*
		 * 200			DO 210 I=1,N
		 * 				S1=S1+X(I)
		 * 				S2=S2+Y(1,I)
		 * 				S3=S3+X(I)*Y(1,I)
		 * 				S4=S4+X(I)*X(I)
		 * 				S5=S5+Y(1,I)*Y(1,I)
		 * 210			CONTINUE
		 * 		T=N*S4-S1*S1
		 * 		S=(N*S3-S1*S2)/T
		 * 		B=(S4*S2-S1*S3)/T
		 * 				DO 220 I=1,N
		 * 				Y(2,I)=S*X(I)+B
		 * 				D=D+(Y(2,I)-Y(1,I))**2
		 * 220	CONTINUE
		 */
		
		//Note that the ** is an exponent
		//Also note that Fortran did not initialize the variables ie, set them to zero.  I will do this as they come up
		S1 = S2 = S3 = S4 = S5 = 0;
		for(int I = 1; I <= N; I++){
			S1 = S1 + X[I];
			S2 = S2 + Y[1][I];
			S3 = S3 + X[I] * Y[1][I];
			S4 = S4 + X[I] * X[I];
			S5 = S5 + Y[1][I] * Y[1][I];	
		}
		T = N*S4 - S1*S1;
		S = (N*S3 - S1*S2)/T;
		B = (S4*S2 - S1*S3)/T;
		D = 0;
		for(int I = 1; I <= N; I++){
			Y[2][I] = S*X[I] + B;
			D = D + Math.pow(Y[2][I] - Y[1][I], 2);
		}
		
		
		
		
		/*
		 * 		D=D/(N-2)
		 * 		E1=DSQRT(D*N/T)
		 * 		E2=DSQRT(D/N*(1+S1*S1/T))
		 * 		R=(N*S3-S1*S2)/
		 * 2	(DSQRT(ABS((N*S4-ABS(S1)**2))*(N*S5-ABS(S2)**2))))
		 * 		WRITE (5,230)
		 * 230	FORMAT(////,10X, 'X-VALUE',20X,'Y-OBS',22X,'Y-CALC')
		 * 		WRITE (5,235)
		 * 235	FORMAT(10X, '=======',20X,'=====',22X,'======')
		 * 				DO 250 I=1,N
		 * 				WRITE (5,240) X(I), Y(1,I), Y(2,I)
		 * 240			FORMAT(F22.10,2F27.10)
		 * 250			CONTINUE
		 */
		
		D = D/(N-2);
		E1 = E2 = 0;
		E1 = Math.sqrt(D*N/T);
		E2 = Math.sqrt(D/N*(1+S1*S1/T));
		//The '2' represents a continuation to the next line
		R=(N*S3-S1*S2)/
				(Math.sqrt(Math.abs(((N*S4-Math.abs(Math.pow(S1, 2))))*(N*S5-Math.abs(Math.pow(S2, 2))))));
		System.out.println("\n\n\n\n          X-VALUE                    Y-OBS                     Y-CALC");
		System.out.println(        "          =======                    =====                     ======");
		for(int I = 1;I <= N; I++){
			tempStr = new String(String.format("%22.10f%27.10f%27.10f", X[I], Y[1][I], Y[2][I]));
			System.out.println(tempStr);
		}
		
		
		
		
		/*
		 * 		WRITE (5,260) S
		 * 260	FORMAT(//,1X,'SLOPE = ',D22.16)
		 * 		WRITE (5,270) E1
		 * 270	FORMAT(1X,'THE ERROR IN THE SLOPE IS +/- ',D22.16)
		 * 		WRITE(5,280) B
		 * 280	FORMAT(//,1X,'INTERCEPT = ',D22.16)
		 * 		WRITE (5,285) E2
		 * 285	FORMAT(1X,'THE ERROR IN THE INTERCEPT IS +/- ',D22.16)
		 * 		LITERL(2)='+'
		 * 		IF (ABS(B).NE.B) LITERL(2)='-'
		 * 		WRITE (5,290)
		 * 290	FORMAT(//,1X,'EQUATION FOR THE BEST LINEAR FIT IS : ')
		 * 		BBAR=ABS(B)
		 * 		WRITE (5,295) S,LITERL(2),BBAR
		 * 295	FORMAT(1X,'Y(X) =',D25.16,' * X ',AL,D25.16,///)
		 * 		WRITE (5,296) R
		 * 296	FORMAT(' LINEAR CORRELATION COEFFICIENT =',D22.16)
		 * 		STOP 'LINEAR... Execution completed'
		 * 		END
		 */
		
		tempStr = new String(String.format("\n\n SLOPE = %22.16e", S));
		System.out.print(tempStr);
		tempStr = new String(String.format(" THE ERROR IN THE SLOPE IS +/- %22.16e", E1));
		System.out.print(tempStr);
		tempStr = new String(String.format("\n\n INTERCEPT =  %22.16e", B));
		System.out.print(tempStr);
		tempStr = new String(String.format(" THE ERROR IN THE INTERCEPT IS +/- %22.16e", E2));
		System.out.print(tempStr);
		LITERL[2] = '+';
		if(Math.abs(B) != B){
			LITERL[2] = '-';
		}
		System.out.print("\n\n EQUATION FOR THE BEST LINEAR FIT IS : ");
		BBAR = Math.abs(B);
		tempStr = new String(String.format(" Y(X) =%25.16e*X " + LITERL[2] + "%25.16e\n\n\n",S,BBAR ));
		System.out.print(tempStr);
		tempStr = new String(String.format(" LINEAR CORRELATION COEFFICIENT =%22.16e",R));
		System.out.print(tempStr);
		
		
		
		reader.close();
		return;
	}
}
