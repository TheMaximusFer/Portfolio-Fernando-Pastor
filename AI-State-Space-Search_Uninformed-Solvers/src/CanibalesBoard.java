package aima.core.environment.canibales;

public class CanibalesBoard {
    public static final int TOTAL_M = 3, TOTAL_C = 3;
    private final int mIzda, cIzda; //En la orilla izquierda.
    private final boolean boteIzda; //true si el bote está a la izquierda

    public CanibalesBoard(int mIzda, int cIzda, boolean boteIzda) {
        this.mIzda = mIzda; 
        this.cIzda = cIzda; 
        this.boteIzda = boteIzda;
    }

    public int mIzda(){return mIzda;}
    
    public int cIzda(){return cIzda;}

    public int mDcha(){return (TOTAL_M - mIzda);}
    
    public int cDcha(){return (TOTAL_C - cIzda);}

    public boolean boteIzda(){return boteIzda;}


    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof CanibalesBoard)) return false;
        CanibalesBoard s = (CanibalesBoard) o;
        return mIzda == s.mIzda && cIzda == s.cIzda && boteIzda == s.boteIzda;
    }

    @Override
    public int hashCode() {
        return java.util.Objects.hash(mIzda, cIzda, boteIzda);
    }

    @Override
    public String toString() {
        return String.format("IZDA: M=%d C=%d %s | DER: M=%d C=%d",
                mIzda, cIzda, boteIzda ? "[BOTE]" : "       ", mDcha(), cDcha());
    }

}
