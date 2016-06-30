# Add project specific ProGuard rules here.
# By default, the flags in this file are appended to flags specified
# in /home/android-developer/Android/Sdk/tools/proguard/proguard-android.txt
# You can edit the include path and order by changing the proguardFiles
# directive in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# Add any project specific keep options here:

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}
-keep class com.google.android.gms.** { *; }
-dontwarn com.google.android.gms.**
-keep class com.facebook.android.** {*;}
-keep class com.crashlytics.sdk.android.** {*;}
-keep class com.twitter.sdk.android.** {*;}
-keep class org.jcodec.** {*;}
-dontwarn com.squareup.okhttp.**
-dontwarn com.google.appengine.api.urlfetch.**
-dontwarn rx.**
-dontwarn retrofit.**
-keepattributes Signature
-keepattributes *Annotation*
-keepattributes SourceFile,LineNumberTable
-printmapping mapping.txt
-keep class com.squareup.okhttp.** { *; }
-keep interface com.squareup.okhttp.** { *; }
-keep class retrofit.** { *; }
-keepclasseswithmembers class * {
    @retrofit.http.* <methods>;
}

-keep class android.support.v4.** { *; }
-keep interface android.support.v4.** { *; }

-keep class android.support.v7.** { *; }
-keep interface android.support.v7.** { *; }
-keep class !android.support.v7.internal.view.menu.**,android.support.** {*;}
-keep class com.smackall.animator.opengl.**{*;}
-keep class com.smackall.animator.NativeCallBackClasses.**{*;}
-keep class com.smackall.animator.DownloadManager.**{*;}

-keep @io.realm.annotations.RealmModule class *
-keep class * extends io.realm.RealmObject
-keep class io.realm.*Proxy{ *;}
-keep class io.realm.ValidationList {* ;}
-dontwarn javax.**
-dontwarn io.realm.**


