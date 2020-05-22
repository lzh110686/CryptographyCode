import java.util.Base64;
import java.security.Key;
import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.spec.SecretKeySpec;
public class AES {
public static final String KEY_ALGORITHM="AES";
public static final String CIPHER_ALGORITHM="AES/ECB/PKCS5Padding";
private static Key toKey(byte[] key) throws Exception{
	//实例化AES密钥
	SecretKey secretKey=new SecretKeySpec(key,KEY_ALGORITHM);
	return secretKey;
	}
public static byte[] decrypt(byte[] data,byte[] key)throws Exception{
	Key k=toKey(key);
	Cipher cipher =Cipher.getInstance(CIPHER_ALGORITHM);
	cipher.init(Cipher.DECRYPT_MODE, k);
	return cipher.doFinal(data);
	}
public static byte[] encrypt(byte[] data,byte[] key)throws Exception{
	Key k=toKey(key);
	Cipher cipher=Cipher.getInstance(CIPHER_ALGORITHM);
	cipher.init(Cipher.ENCRYPT_MODE, k);
	return cipher.doFinal(data);
}
public static byte[] initKey() throws Exception{
	//实例化
	KeyGenerator kg=KeyGenerator.getInstance(KEY_ALGORITHM);
	//AES 要求密钥长度为128位，192位或者256位
	kg.init(128);
	//生成密钥
	SecretKey secretKey=kg.generateKey();
	//获得密钥的二进制编码形式
	return secretKey.getEncoded();
}
public static void main(String[] args) throws Exception {
	String inputStr="AES";
	byte[] inputData=inputStr.getBytes();
	System.out.println("原文：\t"+inputStr);
	byte[] key=AES.initKey();
	System.out.println("密钥：\t"+new String(Base64.getEncoder().encode(key)));
	//加密
	inputData=AES.encrypt(inputData, key);
	System.out.println("加密后：\t"+new String(Base64.getEncoder().encode(inputData)));
	byte[] outputData=AES.decrypt(inputData, key);
	String outputStr=new String(outputData);
	System.out.println("解密后：\t"+outputStr);
//S2Tpe4l3YKuJB8Ew51dAprqX2WKJhRUxM5SolIgqnoY=
//KK7tt4PIdJZkTwz6t0d2at6eEkifcTtsoSHl2J1BmD8=
}
}


















