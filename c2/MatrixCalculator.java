import static java.lang.Math.sqrt;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Locale;
import java.util.Scanner;

public class MatrixCalculator {
    public class Matrix {
        private int ncols;
        private int nrows;
        private float _data[];

        public Matrix(int r, int c) {
            this.ncols = c;
            this.nrows = r;
            _data = new float[c * r];
        }

        public float get(int r, int c) {
            return _data[r * ncols + c];
        }

        public void set(int r, int c, float v) {
            _data[r * ncols + c] = v;
        }

        public int rows() {
            return nrows;
        }

        public int cols() {
            return ncols;
        }

        public void print() {
            System.out.println("[");
            for (int r = 0; r < nrows; r++) {

                for (int c = 0; c < ncols; c++) {
                    System.out.print(get(r, c));
                    System.out.print(" ");
                }

                System.out.println("");
            }
            System.out.println("]\n");
        }
    }

    protected Matrix read(String fname) throws FileNotFoundException {
        File f = new File(fname);
        Scanner scanner = new Scanner(f).useLocale(Locale.ENGLISH);
        Matrix res = null;

        try {
            int rows = scanner.nextInt();
            int cols = scanner.nextInt();
            if (rows < 1 || cols < 1) {
                System.out.println("Bledny format pliku");
                System.exit(-1);
            }
            res = new Matrix(rows, cols);

            for (int r = 0; r < res.rows(); r++) {
                for (int c = 0; c < res.cols(); c++) {
                    res.set(r, c, scanner.nextFloat());
                }
            }
        } catch (Exception e) {
            System.out.println("Bledny format pliku");
            System.exit(-1);
        }

        scanner.close();
        return res;
    }

    protected class MultChunkCalculator extends Thread {
        private Matrix A;
        private Matrix B;
        private Matrix C;
        private int startField;
        private int fieldsNum;
        private float squaresSum;
        private float partialSum;

        public MultChunkCalculator(Matrix A, Matrix B, Matrix C, int startField, int fieldsNum) {
            this.A = A;
            this.B = B;
            this.C = C;
            this.startField = startField;
            this.fieldsNum = fieldsNum;
            this.squaresSum = 0;
            this.partialSum = 0;
        }

        public void run() {
            for (int i = startField; i < startField + fieldsNum; i++) {
                int row = i / B.cols();
                int col = i % B.cols();
                float sum = 0;
                for (int j = 0; j < A.cols(); j++) {
                    sum += A.get(row, j) * B.get(j, col);
                }
                C.set(row, col, sum);
                squaresSum += sum * sum;
                partialSum += sum;
            }
        }

        public float getSquaresSum() {
            return squaresSum;
        }

        public float getPartialSum() {
            return partialSum;
        }
    }

    protected class MultCalculator {
        private Matrix A;
        private Matrix B;
        private Matrix C;
        private float frobeniusNorm;
        private int threadsNum;
        private float partialSums[];

        public MultCalculator(Matrix A, Matrix B, Matrix C, int threadsNum) {
            this.A = A;
            this.B = B;
            this.C = C;
            this.threadsNum = threadsNum;
            this.frobeniusNorm = 0;
            this.partialSums = new float[threadsNum];
        }

        public void calculate() throws InterruptedException {
            MultChunkCalculator[] threads = new MultChunkCalculator[threadsNum];
            int fieldsNum = C.cols() * C.rows();
            int defaultChunkSize = fieldsNum / threadsNum;
            int remainder = fieldsNum % threadsNum;
            int ptr = 0;

            for (int i = 0; i < threadsNum; i++) {
                int chunkSize = remainder-- > 0 ? defaultChunkSize + 1 : defaultChunkSize;
                threads[i] = new MultChunkCalculator(A, B, C, ptr, chunkSize);
                threads[i].start();
                ptr += chunkSize;
            }
            for (int i = 0; i < threadsNum; i++) {
                threads[i].join();
                frobeniusNorm += threads[i].getSquaresSum();
                partialSums[i] = threads[i].getPartialSum();
            }
            frobeniusNorm = (float) sqrt(frobeniusNorm);
        }

        public float getFrobeniusNorm() {
            return frobeniusNorm;
        }

        public float[] getPartialSums() {
            return partialSums;
        }
    }

    protected void start(String[] args) throws FileNotFoundException {
        int threadsNum = args.length > 0 ? Integer.parseInt(args[0]) : 5;

        Matrix A, B;
        A = read("A.txt");
        B = read("B.txt");

        System.out.println("Wczytalem A:");
        A.print();

        System.out.println("Wczytalem B:");
        B.print();

        if (A.cols() != B.rows()) {
            System.out.println("Zle wymiary macierzy");
            return;
        }

        Matrix C = new Matrix(A.rows(), B.cols());
        MultCalculator mltCalculator = new MultCalculator(A, B, C, threadsNum);
        try {
            mltCalculator.calculate();
        } catch (InterruptedException e) {
            System.err.println("Nie udalo sie obliczyc mnozenia macierzy");
            e.printStackTrace();
        }
        System.out.println("Obliczona macierz C:");
        C.print();

        System.out.println("Sumy czesciowe: ");
        float partialSums[] = mltCalculator.getPartialSums();
        float sum = 0;
        for (int i = 0; i < threadsNum; i++) {
            sum += partialSums[i];
            System.out.println(i + ": " + partialSums[i]);
        }
        System.out.println("Suma calkowita: " + sum);

        System.out.println("Norma Frobeniusa: " + mltCalculator.getFrobeniusNorm());
    }

    public static void main(String[] args) {
        MatrixCalculator normCalculator = new MatrixCalculator();

        try {
            normCalculator.start(args);
        } catch (FileNotFoundException e) {
            System.err.println("Cos poszlo nie tak");
            e.printStackTrace();
        }
    }
}
