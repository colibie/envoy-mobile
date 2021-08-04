package org.chromium.net.testing;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * @JNINamespace is used by the JNI generator to create the necessary JNI
 * bindings and expose this method to native code using the specified namespace.
 */
@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
public @interface JNINamespace {
  public String value();
}