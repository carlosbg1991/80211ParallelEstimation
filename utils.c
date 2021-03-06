#include "utils.h"

void hermitian(long double complex **M, int row, int col, long double complex **res){
	for (int r = 0; r < row; r++)
		for(int c = 0 ; c < col ; c++ )
			res[c][r] = creal(M[r][c]) - cimag(M[r][c]);
}

void hermitian_omp(long double complex **M, int row, int col, long double complex **res){
    #pragma omp parallel for num_threads(row)
    for (int r = 0; r < row; r++)
        for(int c = 0 ; c < col ; c++ )
            res[c][r] = creal(M[r][c]) - cimag(M[r][c]);
}

void multiply(long double complex **M1, int row1, int col1, long double complex **M2, int row2, int col2, long double complex **res){
    long double complex sum = 0;
    if (col1 != row2)
        printf("Matrices dimension missmatch\n");
    else {
        for (int c = 0; c < row1; c++) {
            for (int d = 0; d < col2; d++) {
                for (int k = 0; k < row2; k++){
                    sum = sum + M1[c][k]*M2[k][d];
                }        
                res[c][d] = sum;
                sum = 0;
            }
        }
    }
}

void multiply_omp(long double complex **M1, int row1, int col1, long double complex **M2, int row2, int col2, long double complex **res){
	long double complex sum = 0;
    int c, d, k;
	if (col1 != row2)
		printf("Matrices dimension missmatch\n");
	else {
        #pragma omp parallel shared(res) private(sum,c,d,k) 
        {
            #pragma omp for schedule(static)
    		for (c = 0; c < row1; c++) {
    			for (d = 0; d < col2; d++) {
    				for (k = 0; k < row2; k++){
    					sum = sum + M1[c][k]*M2[k][d];
                    }        
    				res[c][d] = sum;
    				sum = 0;
    			}
    	    }
        }
    }
}

void multiplyVxVeqM(long double complex **M1, int row1, int col1, long double complex **M2, int row2, int col2, long double complex **res){
    if (col1 != row2)
        printf("Matrices dimension missmatch\n");
    else {
        for(int r=0; r<row1; r++){
            for(int c=0; c<col2; c++){
                res[r][c] = M1[r][0]*M2[0][c];
            }
        }
    }
}

void multiplyVxVeqM_omp(long double complex **M1, int row1, int col1, long double complex **M2, int row2, int col2, long double complex **res){
    int r, c;
    if (col1 != row2)
        printf("Matrices dimension missmatch\n");
    else {
        #pragma omp parallel shared(res) private(r,c) 
        {
            #pragma omp for schedule(static)
            for(r=0; r<row1; r++){
                for(c=0; c<col2; c++){
                    res[r][c] = M1[r][0]*M2[0][c];
                }
            }
        }
    }
}

void identity(long double complex **Identity,int size,double scalar){
	for (int c = 0; c < size; c++) {
		for (int r = 0; r < size; r++) {
			if(c==r)
				Identity[r][c] = scalar;
			else
				Identity[r][c] = 0.0;
		}
	}
}

void identity_omp(long double complex **Identity,int size,double scalar){
    int r, c;
    #pragma omp parallel shared(Identity) private(r,c) 
    {
        #pragma omp for schedule(static)
        for (c = 0; c < size; c++) {
            for (r = 0; r < size; r++) {
                if(c==r)
                    Identity[r][c] = scalar;
                else
                    Identity[r][c] = 0.0;
            }
        }
    }    
}

void addition(long double complex **M1, int row1, int col1, long double complex **M2, int row2, int col2, long double complex **res){
  if ((row1 != row2) || (col1 != col2))
      printf("Matrices dimension missmatch\n");
  else {
		for (int c = 0; c < col1; c++) {
			for (int r = 0; r < row1; r++) {
				res[r][c] = M1[r][c]+M1[r][c];
			}
		}
	}
}

void addition_omp(long double complex **M1, int row1, int col1, long double complex **M2, int row2, int col2, long double complex **res){
    int r, c;
  if ((row1 != row2) || (col1 != col2))
      printf("Matrices dimension missmatch\n");
  else {
        #pragma omp parallel shared(res) private(r,c) 
        {
            #pragma omp for schedule(static)
            for (c = 0; c < col1; c++) {
                for (r = 0; r < row1; r++) {
                    res[r][c] = M1[r][c]+M1[r][c];
                }
            }
        }
    }
}

// matrix inversioon --- https://chi3x10.wordpress.com/2008/05/28/calculate-matrix-inversion-in-c/
void inverse(long double complex **A, int order, long double complex **Y) {
    // get the determinant of a
    //long double complex det = 1.0/CalcDeterminant(A,order);        // Regular Method
    long double complex det = 1.0/determinant_impl_rec(A,order);         // Cramer Method
	long double complex temporal;
    int i,j;
 
    // memory allocation
    long double complex *temp = new long double complex [(order-1)*(order-1)];
    long double complex **minor = new long double complex *[order-1];
    for(int i=0;i<order-1;i++)
        minor[i] = temp+(i*(order-1));
 
    for(j=0;j<order;j++)
    {
        for(i=0;i<order;i++)
        {
            // get the co-factor (matrix) of A(j,i)
            GetMinor(A,minor,j,i,order);
            // Y[i][j] = det*CalcDeterminant(minor,order-1);          // Regular Method
            Y[i][j] = det*determinant_impl_rec(minor,order-1);        // Cramer Method
            if( (i+j)%2 == 1)
				Y[i][j] = (-1)*Y[i][j];
        }
    }

    // release memory
    delete [] temp;
    delete [] minor;
}

void inverse_omp(long double complex **A, int order, long double complex **Y) {
    // get the determinant of a
    //long double complex det = 1.0/CalcDeterminant(A,order);        // Regular Method
    long double complex det = 1.0/determinant_impl_omp(A,order);         // Cramer Method
    long double complex temporal;
    int i,j;
 
    // memory allocation
    long double complex *temp = new long double complex [(order-1)*(order-1)];
    long double complex **minor = new long double complex *[order-1];
    for(int i=0;i<order-1;i++)
        minor[i] = temp+(i*(order-1));

    for(j=0;j<order;j++) {
        for(i=0;i<order;i++) {
            // get the co-factor (matrix) of A(j,i)
            GetMinor(A,minor,j,i,order);
            // Y[i][j] = det*CalcDeterminant(minor,order-1);      // Regular Method recursive seq
            Y[i][j] = det*determinant_impl_omp(minor,order-1);    // Cramer Method recursive seq
            if( (i+j)%2 == 1)
                Y[i][j] = (-1)*Y[i][j];
        }
    }

    // release memory
    delete [] temp;
    delete [] minor;
}

void inverse_mpi_old(long double complex **A, int order, long double complex **Y, Common_PS *commonPS, int argc, char *argv[]) {
    // get the determinant of a
    //long double complex det = 1.0/CalcDeterminant(A,order);            // Regular Method
    long double complex det = 1.0/determinant_impl_rec(A,order);         // Cramer Method
    long double complex temporal, res;
    long double res_real[SAMPUTIL][3], res_imag[SAMPUTIL][3];
    int i,j;
 
    // memory allocation
    long double complex *temp = new long double complex [(order-1)*(order-1)];
    long double complex **minor = new long double complex *[order-1];

    for(int i=0;i<order-1;i++)
        minor[i] = temp+(i*(order-1));

    if(commonPS->rank!=0){
        for(j=0;j<order;j++) {
            GetMinor(A,minor,j,commonPS->rank,order);
            res = det*determinant_impl_rec(minor,order-1);              // Cramer Method Y[commonPS->rank][j] = res1
            if( (commonPS->rank+j)%2 == 1) {res = (-1)*res; }
            res_real[j][0] = creal(res); res_imag[j][0] = cimag(res);

            GetMinor(A,minor,j,commonPS->rank+20,order);
            res = det*determinant_impl_rec(minor,order-1);              // Cramer Method Y[commonPS->rank][j] = res1
            if( (commonPS->rank+20+j)%2 == 1) {res = (-1)*res; }
            res_real[j][1] = creal(res); res_imag[j][1] = cimag(res);

            if(commonPS->rank<13){
                GetMinor(A,minor,j,commonPS->rank+40,order);
                res = det*determinant_impl_rec(minor,order-1);          // Cramer Method Y[commonPS->rank][j] = res1
                if( (commonPS->rank+40+j)%2 == 1) {res = (-1)*res; }
                res_real[j][2] = creal(res); res_imag[j][2] = cimag(res);
            }
        }

        MPI_Send(res_real, order*3, MPI_LONG_DOUBLE, 0, commonPS->tag1, MPI_COMM_WORLD);
        MPI_Send(res_imag, order*3, MPI_LONG_DOUBLE, 0, commonPS->tag2, MPI_COMM_WORLD);

    } else {

        // Proc 0 does its part of the job
        for(j=0;j<order;j++) {
            GetMinor(A,minor,j,commonPS->rank,order);
            Y[0][j] = det*determinant_impl_rec(minor,order-1);           // Cramer Method Y[commonPS->rank][j] = res1
            if( (0+j)%2 == 1) {Y[0][j] = (-1)*Y[0][j]; }
            GetMinor(A,minor,j,commonPS->rank+20,order);
            Y[20][j] = det*determinant_impl_rec(minor,order-1);        // Cramer Method Y[commonPS->rank][j] = res1
            if( (20+j)%2 == 1) {Y[20][j] = (-1)*Y[20][j]; }
            GetMinor(A,minor,j,commonPS->rank+40,order);
            Y[40][j] = det*determinant_impl_rec(minor,order-1);        // Cramer Method Y[commonPS->rank][j] = res1
            if( (40+j)%2 == 1) {Y[40][j] = (-1)*Y[40][j]; }
        }

        // Proc 0 receives results from rest of processes
        for(int proc=1 ; proc<commonPS->numprocs; proc++){
            MPI_Recv(res_real, order*3, MPI_LONG_DOUBLE, proc, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
            MPI_Recv(res_imag, order*3, MPI_LONG_DOUBLE, proc, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
            for(int j=0; j<SAMPUTIL; j++){
                Y[proc+0][j] = res_real[j][0] + I*res_imag[j][0];
                Y[proc+20][j] = res_real[j][1] + I*res_imag[j][1];
                if(proc<13)
                    Y[proc+40][j] = res_real[j][2] + I*res_imag[j][2];
            }
            
        }
    }

    // release memory
    delete [] temp;
    delete [] minor;
}

void inverse_mpi(long double complex **A, int order, long double complex **Y, Common_PS *commonPS, int argc, char *argv[]) {
    // get the determinant of a
    long double complex det = 1.0/determinant_impl_rec(A,order);    // Cramer Method Sequential
    long double complex temporal, res;
    int i,j;

    int numTasks1 = ceil((double) SAMPUTIL/commonPS->numprocs);
    int numTasks2 = floor((double) SAMPUTIL/commonPS->numprocs);
    int set1 = SAMPUTIL%commonPS->numprocs;
    long double res_real[SAMPUTIL][numTasks1], res_imag[SAMPUTIL][numTasks1];
 
    // memory allocation
    long double complex *temp = new long double complex [(order-1)*(order-1)];
    long double complex **minor = new long double complex *[order-1];

    for(int i=0;i<order-1;i++)
        minor[i] = temp+(i*(order-1));

    if(commonPS->rank!=0){
        if(commonPS->rank<set1){
            // set 1
            for(j=0;j<order;j++) {
                for(int task=0; task<numTasks1; task++){
                    GetMinor(A,minor,j,commonPS->rank+task*commonPS->numprocs,order);
                    res = det*determinant_impl_rec(minor,order-1);              // Cramer Method Y[commonPS->rank][j] = res1
                    if( (commonPS->rank+task*commonPS->numprocs+j)%2 == 1) {res = (-1)*res; }
                    res_real[j][task] = creal(res); res_imag[j][task] = cimag(res);
                }
            }
        } else {
            // set 2
            for(j=0;j<order;j++) {
                for(int task=0; task<numTasks2; task++){
                    GetMinor(A,minor,j,commonPS->rank+task*commonPS->numprocs,order);
                    res = det*determinant_impl_rec(minor,order-1);              // Cramer Method Y[commonPS->rank][j] = res1
                    if( (commonPS->rank+task*commonPS->numprocs+j)%2 == 1) {res = (-1)*res; }
                    res_real[j][task] = creal(res); res_imag[j][task] = cimag(res);
                }
            }
        }

        printf("Proc %d sends to 0 \n", commonPS->rank);
        MPI_Send(res_real, order*numTasks1, MPI_LONG_DOUBLE, 0, commonPS->tag1, commonPS->comm);
        MPI_Send(res_imag, order*numTasks1, MPI_LONG_DOUBLE, 0, commonPS->tag2, commonPS->comm);

    } else {
        // Proc 0 does its part of the job
        for(j=0;j<order;j++) {
            for(int task=0; task<numTasks1; task++){
                GetMinor(A,minor,j,0+task*commonPS->numprocs,order);
                Y[0+task*commonPS->numprocs][j] = det*determinant_impl_rec(minor,order-1);              // Cramer Method Y[commonPS->rank][j] = res1
                if( (0+task*commonPS->numprocs+j)%2 == 1) {Y[0+task*commonPS->numprocs][j] = (-1)*Y[0+task*commonPS->numprocs][j]; }
            }
            printf("\t\tProc 0 finished round %d\n",j);
        }

        // Proc 0 receives results from rest of processes
        for(int proc=1 ; proc<commonPS->numprocs; proc++){
            printf("\t\tProc 0 recv from %d\n",proc);
            MPI_Recv(res_real, order*numTasks1, MPI_LONG_DOUBLE, proc, commonPS->tag1, commonPS->comm, &commonPS->status);
            MPI_Recv(res_imag, order*numTasks1, MPI_LONG_DOUBLE, proc, commonPS->tag2, commonPS->comm, &commonPS->status);
            if(proc<set1){
                for(int j=0; j<SAMPUTIL; j++){
                    for(int task=0; task<numTasks1; task++){
                        Y[proc + task*commonPS->numprocs][j] = res_real[j][task] + I*res_imag[j][task];
                    }
                }
            } else {
                for(int j=0; j<SAMPUTIL; j++){
                    for(int task=0; task<numTasks2; task++){
                        Y[proc + task*commonPS->numprocs][j] = res_real[j][task] + I*res_imag[j][task];
                    }
                }
            }
        }
    }

    // release memory
    delete [] temp;
    delete [] minor;
}

void inverse_mpi_omp(long double complex **A, int order, long double complex **Y, Common_PS *commonPS, int argc, char *argv[]) {
    // get the determinant of A
    long double complex det = 1.0/determinant_impl_omp(A,order);    // Cramer Method OpenMP
    long double complex temporal, res;
    int i,j;

    int numTasks1 = ceil((double) SAMPUTIL/commonPS->numprocs);
    int numTasks2 = floor((double) SAMPUTIL/commonPS->numprocs);
    int set1 = SAMPUTIL%commonPS->numprocs;
    long double res_real[SAMPUTIL][numTasks1], res_imag[SAMPUTIL][numTasks1];
 
    // memory allocation
    long double complex *temp = new long double complex [(order-1)*(order-1)];
    long double complex **minor = new long double complex *[order-1];

    for(int i=0;i<order-1;i++)
        minor[i] = temp+(i*(order-1));

    if(commonPS->rank!=0){
        if(commonPS->rank<set1){
            // set 1
            for(j=0;j<order;j++) {
                for(int task=0; task<numTasks1; task++){
                    GetMinor(A,minor,j,commonPS->rank+task*commonPS->numprocs,order);
                    res = det*determinant_impl_omp(minor,order-1);              // Cramer Method OpenMP Y[commonPS->rank][j] = res1
                    if( (commonPS->rank+task*commonPS->numprocs+j)%2 == 1) {res = (-1)*res; }
                    res_real[j][task] = creal(res); res_imag[j][task] = cimag(res);
                }
            }
        } else {
            // set 2
            for(j=0;j<order;j++) {
                for(int task=0; task<numTasks2; task++){
                    GetMinor(A,minor,j,commonPS->rank+task*commonPS->numprocs,order);
                    res = det*determinant_impl_omp(minor,order-1);              // Cramer Method OpenMP Y[commonPS->rank][j] = res1
                    if( (commonPS->rank+task*commonPS->numprocs+j)%2 == 1) {res = (-1)*res; }
                    res_real[j][task] = creal(res); res_imag[j][task] = cimag(res);
                }
            }
        }

        // printf("Proc %d sends to 0 \n", commonPS->rank);
        MPI_Send(res_real, order*numTasks1, MPI_LONG_DOUBLE, 0, commonPS->tag1, commonPS->comm);
        MPI_Send(res_imag, order*numTasks1, MPI_LONG_DOUBLE, 0, commonPS->tag2, commonPS->comm);

    } else {
        // Proc 0 does its part of the job
        for(j=0;j<order;j++) {
            for(int task=0; task<numTasks1; task++){
                GetMinor(A,minor,j,0+task*commonPS->numprocs,order);
                Y[0+task*commonPS->numprocs][j] = det*determinant_impl_omp(minor,order-1);              // Cramer Method Y[commonPS->rank][j] = res1
                if( (0+task*commonPS->numprocs+j)%2 == 1) {Y[0+task*commonPS->numprocs][j] = (-1)*Y[0+task*commonPS->numprocs][j]; }
            }
            // printf("\t\tProc 0 finished round %d\n",j);
        }

        // Proc 0 receives results from rest of processes
        for(int proc=1 ; proc<commonPS->numprocs; proc++){
            // printf("\t\tProc 0 recv from %d\n",proc);
            MPI_Recv(res_real, order*numTasks1, MPI_LONG_DOUBLE, proc, commonPS->tag1, commonPS->comm, &commonPS->status);
            MPI_Recv(res_imag, order*numTasks1, MPI_LONG_DOUBLE, proc, commonPS->tag2, commonPS->comm, &commonPS->status);
            if(proc<set1){
                for(int j=0; j<SAMPUTIL; j++){
                    for(int task=0; task<numTasks1; task++){
                        Y[proc + task*commonPS->numprocs][j] = res_real[j][task] + I*res_imag[j][task];
                    }
                }
            } else {
                for(int j=0; j<SAMPUTIL; j++){
                    for(int task=0; task<numTasks2; task++){
                        Y[proc + task*commonPS->numprocs][j] = res_real[j][task] + I*res_imag[j][task];
                    }
                }
            }
        }

        // printf("\tRoot in MMSE finished\n");
    }

    // release memory
    delete [] temp;
    delete [] minor;
}

// calculate the cofactor of element (row,col)
int GetMinor(long double complex **src, long double complex **dest, int row, int col, int order) {
    // Indicate which col and row is being copied to dest
    int colCount=0,rowCount=0;
 
    for(int i = 0; i < order; i++ ) {
        if( i != row ) {
            colCount = 0;
            for(int j = 0; j < order; j++ ) {
                // when j is not the element
                if( j != col ) {
                    dest[rowCount][colCount] = src[i][j];
                    colCount++;
                }
            }
            rowCount++;
        }
    }

    return 1;
}

// Calculate the determinant recursively.
long double complex CalcDeterminant( long double complex **mat, int order) {
    // order must be >= 0
    // stop the recursion when matrix is a single element
    if(order > 10)
        printf("calcDeterminant - order %d \n",order);
    if( order == 1 )
        return mat[0][0];
 
    // the determinant value
    long double complex det = 0.0 + 0.0*I;
 
    // allocate the cofactor matrix
    long double complex **minor;
    minor = new long double complex *[order-1];
    for(int i=0;i<order-1;i++)
        minor[i] = new long double complex [order-1];
 
    for(int i = 0; i < order; i++ ) {
        // get minor of element (0,i)
        GetMinor( mat, minor, 0, i , order);
        // the recusion is here!
 
        det += (i%2==1?-1.0:1.0) * mat[0][i] * CalcDeterminant(minor,order-1);
        //det += pow( -1.0, i ) * mat[0][i] * CalcDeterminant( minor,order-1 );
    }
 
    // release memory
    for(int i=0;i<order-1;i++)
        delete [] minor[i];
    delete [] minor;
 
    return det;
}

void fft_impl(double data[], int nn, int isign)
{
    int n, mmax, m, j, istep, i;
    double wtemp, wr, wpr, wpi, wi, theta;
    double tempr, tempi;

    n = nn << 1;
    j = 1;
    for (i = 1; i < n; i += 2) {
    if (j > i) {
        tempr = data[j];     data[j] = data[i];     data[i] = tempr;
        tempr = data[j+1]; data[j+1] = data[i+1]; data[i+1] = tempr;
    }
    m = n >> 1;
    while (m >= 2 && j > m) {
        j -= m;
        m >>= 1;
    }
    j += m;
    }
    mmax = 2;
    while (n > mmax) {
    istep = 2*mmax;
    theta = TWOPI/(isign*mmax);
    wtemp = sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi = sin(theta);
    wr = 1.0;
    wi = 0.0;
    for (m = 1; m < mmax; m += 2) {
        for (i = m; i <= n; i += istep) {
        j =i + mmax;
        tempr = wr*data[j]   - wi*data[j+1];
        tempi = wr*data[j+1] + wi*data[j];
        data[j]   = data[i]   - tempr;
        data[j+1] = data[i+1] - tempi;
        data[i] += tempr;
        data[i+1] += tempi;
        }
        wr = (wtemp = wr)*wpr - wi*wpi + wr;
        wi = wi*wpr + wtemp*wpi + wi;
    }
    mmax = istep;
    }
}

// Calculate the determinant recursively.
long double complex determinant_impl_rec( long double complex **mat, int order) {

    long double complex **SubMatrix = new long double complex*[order-1];
    long double complex det;
    for (int i = 0; i < order-1; i++) {
        SubMatrix[i] = new long double complex[order-1];
    }
    if( order == 1 ){
        det = mat[0][0];
    } else if(order == 2){
        det = mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0];
    } else{
        for(int i=1 ; i<order ; i++){
            for(int j=1 ; j<order ; j++){
                SubMatrix[i-1][j-1] = mat[i][j] - (mat[i][0] * mat[0][j] / mat[0][0]);
            }
        }
        det = mat[0][0]*determinant_impl_rec(SubMatrix,order-1);
    }

    // release memory
    for(int i=0;i<order-1;i++)
        delete [] SubMatrix[i];
    delete [] SubMatrix;

    return det;
}

// Calculate the determinant sequentially.
long double complex determinant_impl( long double complex **mat, int order) {
    long double complex tmp_frac, det = 1;
    int last_row;
    int i, j, k;
    int num_swaps;
    bool zero_on_diag, swapped;

    // default to success
    num_swaps = 0;
    zero_on_diag = false;
    
    last_row = order;

    // row 0 stays unaltered
    for (i=1; i < last_row; i++) {      // last_row = mat_size unless get an all zero row
        for (k=0; k < i; k++) {
            tmp_frac = mat[i][k] / mat[k][k];
            for (j = k+1; j < order; j++) {
                mat[i][j] = mat[i][j] - (tmp_frac * mat[k][j]);
            }
        }

        if(mat[i][i] == 0) {
            /* It is now possible we set a diagonal element to zero
            so we shall do column swaps to fix this
            If this fails then row i is all zeros, so
            there would be a zero on the diagonal somewhere anyway,
            and we will leave it here. */
    
            j = i+1; // all cols left of col[i] will have zeros in row[i]
            swapped = false;
            while ((!swapped) && (j < order)) {
                if ( mat[i][j]!=0 ) {
                    swap_cols(mat, order, i, j);
                    swapped = true;
                    num_swaps++;
                }
                j++;
            }
            if (!swapped) {
                /* Could just return zero here
                instead swap this row with current "last" row
                and decrement number of rows to look at
                (as the last ones will be all zero) */
                zero_on_diag = true;
                swap_rows(mat,order, i, last_row - 1);
                last_row--;
            }
        } // end if [i][i] is zero

    } // end for i

    for(int i = 0; i < order; ++i) {
        det *= mat[i][i];
    }

    return det;
}

// Calculate the determinant sequentially in OpenMP.
long double complex determinant_impl_omp( long double complex **mat, int order) {
    long double complex tmp_frac, det = 1;
    int last_row;
    int i, j, k;
    int num_swaps;
    bool zero_on_diag, swapped;

    // default to success
    num_swaps = 0;
    zero_on_diag = false;
    
    last_row = order;

    // row 0 stays unaltered
    #pragma omp parallel for num_threads(THREADS_TEST) schedule(static)
    for (i=1; i < last_row; i++) {      // last_row = mat_size unless get an all zero row
        
        // #pragma omp parallel for num_threads(i) schedule(static)
        //#pragma omp parallel for num_threads(THREADS_TEST) schedule(static)
        for (k=0; k < i; k++) {
            tmp_frac = mat[i][k] / mat[k][k];
            for (j = k+1; j < order; j++) {
                mat[i][j] = mat[i][j] - (tmp_frac * mat[k][j]);
            }
        }

        if(mat[i][i] == 0) {
            /* It is now possible we set a diagonal element to zero
            so we shall do column swaps to fix this
            If this fails then row i is all zeros, so
            there would be a zero on the diagonal somewhere anyway,
            and we will leave it here. */
    
            j = i+1; // all cols left of col[i] will have zeros in row[i]
            swapped = false;
            while ((!swapped) && (j < order)) {
                if ( mat[i][j]!=0 ) {
                    swap_cols(mat, order, i, j);
                    swapped = true;
                    num_swaps++;
                }
                j++;
            }
            if (!swapped) {
                /* Could just return zero here
                instead swap this row with current "last" row
                and decrement number of rows to look at
                (as the last ones will be all zero) */
                zero_on_diag = true;
                swap_rows(mat,order, i, last_row - 1);
                last_row--;
            }
        } // end if [i][i] is zero

    } // end for i

    for(int i = 0; i < order; ++i) {
        det *= mat[i][i];
    }

    return det;
}


void printVect(long double complex *vec, int size,char *name){
	for(int k=0 ; k<size ; k++){
        printf("%s[%d] = %f + %fi \n",name,k,creal(vec[k]),cimag(vec[k]));
    }
}

void printMatrix(long double complex **mat, int rows, int cols, char *name){
    for(int r=0 ; r<rows ; r++) {
        for(int c=0 ; c<cols ; c++){
            printf("%s[%d][%d] = %f + %fi \n",name,r,c,creal(mat[r][c]),cimag(mat[r][c]));
        }
    }
}

void printMatrixReal(long double **mat, int rows, int cols, char *name){
    for(int c=0 ; c<cols ; c++){
        for(int r=0 ; r<rows ; r++) {
            printf("%s[%d][%d] = %Lf \n",name,r,c,mat[r][c]);
        }
    }
}

void printMatrix2(long double complex **mat, int rows, int cols, char *name){
    for(int r=0 ; r<rows ; r++) {
        for(int c=0 ; c<cols ; c++){
            printf("%.0f + %.0fi\t",creal(mat[r][c]),cimag(mat[r][c]));
        }
        printf("\n");
    }
}

double sinc(double input ){
    if(input != 0){
        return sin(PI*input)/(PI*input);
    } else {
        return 1;
    }
}

void swap_rows(long double complex **mat, int order, int row1, int row2){
    long double complex temp;
    for (int i=0; i<order; ++i){
        temp = mat[row2][i];
        mat[row2][i] = mat[row1][i];
        mat[row1][i] = temp;
    }
}

void swap_cols(long double complex **mat, int order, int row1, int row2){
    long double complex temp;
    for (int i=0; i<order; ++i){
        temp = mat[i][row2];
        mat[i][row2] = mat[i][row1];
        mat[i][row1] = temp;
    }
}

void test_master(int numprocs){
    printf("I am the master node \n");
    int rec, tag1= 1;
    MPI_Status status;

    for(int src=1; src<numprocs; src++){
        MPI_Recv(&rec, 1, MPI_INT, src, tag1, MPI_COMM_WORLD, &status);
    }
}

void test_slave(int rank){
    printf("I am process %d node \n", rank);
    int tag1= 1;
    MPI_Send(&rank, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD);
}


void multiply_mpi(long double complex **M1, int row1, int col1, long double complex *vec, int col2, long double complex **res, int from, int to){
    for (int i=from; i<to; i++){ 
        for (int j=0; j<row1; j++) {
            res[i][j]=0;
            for (int k=0; k<col2; k++)
                res[i][j] += M1[i][k]*M1[k][j];
        }
    }
}

void complexToDouble(int order, long double complex **matrix, long double **matrix_Re, long double **matrix_Im){
    for (int r = 0; r < order; r++){
        for(int c = 0 ; c < order ; c++ ){
            matrix_Re[r][c] = creal(matrix[r][c]);
            matrix_Im[r][c] = cimag(matrix[r][c]);
        }
    }
}

void doubleToComplex(int order, long double complex **matrix, long double **matrix_Re, long double **matrix_Im){
    for (int r = 0; r < order; r++){
        for(int c = 0 ; c < order ; c++ ){
            matrix[r][c] = matrix_Re[r][c] + I*matrix_Im[r][c];
        }
    }
}

int malloc2dLongDouble(long double ***array, int n, int m) {

    /* allocate the n*m contiguous items */
    long double *p = (long double *)malloc(n*m*sizeof(long double));
    if (!p) return -1;

    /* allocate the row pointers into the memory */
    (*array) = (long double **)malloc(n*sizeof(long double*));
    if (!(*array)) {
       free(p);
       return -1;
    }

    /* set up the pointers into the contiguous memory */
    for (int i=0; i<n; i++) 
       (*array)[i] = &(p[i*m]);

    return 0;
}

int malloc2dLongDoubleComplex(long double complex ***array, int n, int m) {

    /* allocate the n*m contiguous items */
    long double complex *p = (long double complex*)malloc(n*m*sizeof(long double complex));
    if (!p) return -1;

    /* allocate the row pointers into the memory */
    (*array) = (long double complex**)malloc(n*sizeof(long double complex*));
    if (!(*array)) {
       free(p);
       return -1;
    }

    /* set up the pointers into the contiguous memory */
    for (int i=0; i<n; i++) 
       (*array)[i] = &(p[i*m]);

    return 0;
}

int free2dLongDouble(long double ***array) {
    /* free the memory - the first element of the array is at the start */
    free(&((*array)[0][0]));

    /* free the pointers into the memory */
    free(*array);

    return 0;
}

int free2dLongDoubleComplex(long double complex***array) {
    /* free the memory - the first element of the array is at the start */
    free(&((*array)[0][0]));

    /* free the pointers into the memory */
    free(*array);

    return 0;
}