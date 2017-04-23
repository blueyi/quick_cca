/*
 * TestCCA.java
 * Copyright (C) 2017 blueyi <blueyi@blueyi-mint>
 *
 * Distributed under terms of the MIT license.
 */
package win.dutoe;

public class TestCCA
{
    static {
        System.loadLibrary("IceAnalysis");
    }
    private native void libIceAnalysis(String input);
    public static void main (String argv[]) {
        if (argv.length < 1) {
            System.out.println("Input Error!");
            return;
        }
        new TestCCA().libIceAnalysis(argv[0]);
    }
}

