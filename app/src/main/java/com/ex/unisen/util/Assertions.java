package com.ex.unisen.util;

import android.os.Looper;
import android.text.TextUtils;

import androidx.annotation.Nullable;

public class Assertions {

    public static final boolean ASSERTIONS_ENABLED = true;
    
    private Assertions() {}

    /**
     * Throws {@link IllegalArgumentException} if {@code expression} evaluates to false.
     *
     * @param expression The expression to evaluate.
     * @throws IllegalArgumentException If {@code expression} is false.
     */
    public static void checkArgument(boolean expression) {
        if (ASSERTIONS_ENABLED && !expression) {
            throw new IllegalArgumentException();
        }
    }

    /**
     * Throws {@link IllegalArgumentException} if {@code expression} evaluates to false.
     *
     * @param expression The expression to evaluate.
     * @param errorMessage The exception message if an exception is thrown. The message is converted
     *     to a {@link String} using {@link String#valueOf(Object)}.
     * @throws IllegalArgumentException If {@code expression} is false.
     */
    public static void checkArgument(boolean expression, Object errorMessage) {
        if (ASSERTIONS_ENABLED && !expression) {
            throw new IllegalArgumentException(String.valueOf(errorMessage));
        }
    }

    /**
     * Throws {@link IndexOutOfBoundsException} if {@code index} falls outside the specified bounds.
     *
     * @param index The index to test.
     * @param start The start of the allowed range (inclusive).
     * @param limit The end of the allowed range (exclusive).
     * @return The {@code index} that was validated.
     * @throws IndexOutOfBoundsException If {@code index} falls outside the specified bounds.
     */
    public static int checkIndex(int index, int start, int limit) {
        if (index < start || index >= limit) {
            throw new IndexOutOfBoundsException();
        }
        return index;
    }

    /**
     * Throws {@link IllegalStateException} if {@code expression} evaluates to false.
     *
     * @param expression The expression to evaluate.
     * @throws IllegalStateException If {@code expression} is false.
     */
    public static void checkState(boolean expression) {
        if (ASSERTIONS_ENABLED && !expression) {
            throw new IllegalStateException();
        }
    }

    /**
     * Throws {@link IllegalStateException} if {@code expression} evaluates to false.
     *
     * @param expression The expression to evaluate.
     * @param errorMessage The exception message if an exception is thrown. The message is converted
     *     to a {@link String} using {@link String#valueOf(Object)}.
     * @throws IllegalStateException If {@code expression} is false.
     */
    public static void checkState(boolean expression, Object errorMessage) {
        if (ASSERTIONS_ENABLED && !expression) {
            throw new IllegalStateException(String.valueOf(errorMessage));
        }
    }

    /**
     * Throws {@link NullPointerException} if {@code reference} is null.
     *
     * @param <T> The type of the reference.
     * @param reference The reference.
     * @return The non-null reference that was validated.
     * @throws NullPointerException If {@code reference} is null.
     */
    @SuppressWarnings({"contracts.postcondition.not.satisfied", "return.type.incompatible"})
    public static <T> T checkNotNull(@Nullable T reference) {
        if (ASSERTIONS_ENABLED && reference == null) {
            throw new NullPointerException();
        }
        return reference;
    }

    /**
     * Throws {@link NullPointerException} if {@code reference} is null.
     *
     * @param <T> The type of the reference.
     * @param reference The reference.
     * @param errorMessage The exception message to use if the check fails. The message is converted
     *     to a string using {@link String#valueOf(Object)}.
     * @return The non-null reference that was validated.
     * @throws NullPointerException If {@code reference} is null.
     */
    @SuppressWarnings({"contracts.postcondition.not.satisfied", "return.type.incompatible"})
    public static <T> T checkNotNull(@Nullable T reference, Object errorMessage) {
        if (ASSERTIONS_ENABLED && reference == null) {
            throw new NullPointerException(String.valueOf(errorMessage));
        }
        return reference;
    }

    /**
     * Throws {@link IllegalArgumentException} if {@code string} is null or zero length.
     *
     * @param string The string to check.
     * @return The non-null, non-empty string that was validated.
     * @throws IllegalArgumentException If {@code string} is null or 0-length.
     */
    @SuppressWarnings({"contracts.postcondition.not.satisfied", "return.type.incompatible"})
    public static String checkNotEmpty(@Nullable String string) {
        if (ASSERTIONS_ENABLED && TextUtils.isEmpty(string)) {
            throw new IllegalArgumentException();
        }
        return string;
    }

    /**
     * Throws {@link IllegalArgumentException} if {@code string} is null or zero length.
     *
     * @param string The string to check.
     * @param errorMessage The exception message to use if the check fails. The message is converted
     *     to a string using {@link String#valueOf(Object)}.
     * @return The non-null, non-empty string that was validated.
     * @throws IllegalArgumentException If {@code string} is null or 0-length.
     */
    @SuppressWarnings({"contracts.postcondition.not.satisfied", "return.type.incompatible"})
    public static String checkNotEmpty(@Nullable String string, Object errorMessage) {
        if (ASSERTIONS_ENABLED && TextUtils.isEmpty(string)) {
            throw new IllegalArgumentException(String.valueOf(errorMessage));
        }
        return string;
    }

    /**
     * Throws {@link IllegalStateException} if the calling thread is not the application's main
     * thread.
     *
     * @throws IllegalStateException If the calling thread is not the application's main thread.
     */
    public static void checkMainThread() {
        if (ASSERTIONS_ENABLED && Looper.myLooper() != Looper.getMainLooper()) {
            throw new IllegalStateException("Not in applications main thread");
        }
    }
}
