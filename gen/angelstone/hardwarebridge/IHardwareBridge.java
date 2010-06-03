/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: /home/angelstone/Work/Cpp/hardware_bridge/src/angelstone/hardwarebridge/IHardwareBridge.aidl
 */
package angelstone.hardwarebridge;
import java.lang.String;
import android.os.RemoteException;
import android.os.IBinder;
import android.os.IInterface;
import android.os.Binder;
import android.os.Parcel;
// Declare the interface.

public interface IHardwareBridge extends android.os.IInterface
{
/** Local-side IPC implementation stub class. */
public static abstract class Stub extends android.os.Binder implements angelstone.hardwarebridge.IHardwareBridge
{
private static final java.lang.String DESCRIPTOR = "angelstone.hardwarebridge.IHardwareBridge";
/** Construct the stub at attach it to the interface. */
public Stub()
{
this.attachInterface(this, DESCRIPTOR);
}
/**
 * Cast an IBinder object into an IHardwareBridge interface,
 * generating a proxy if needed.
 */
public static angelstone.hardwarebridge.IHardwareBridge asInterface(android.os.IBinder obj)
{
if ((obj==null)) {
return null;
}
android.os.IInterface iin = (android.os.IInterface)obj.queryLocalInterface(DESCRIPTOR);
if (((iin!=null)&&(iin instanceof angelstone.hardwarebridge.IHardwareBridge))) {
return ((angelstone.hardwarebridge.IHardwareBridge)iin);
}
return new angelstone.hardwarebridge.IHardwareBridge.Stub.Proxy(obj);
}
public android.os.IBinder asBinder()
{
return this;
}
@Override public boolean onTransact(int code, android.os.Parcel data, android.os.Parcel reply, int flags) throws android.os.RemoteException
{
switch (code)
{
case INTERFACE_TRANSACTION:
{
reply.writeString(DESCRIPTOR);
return true;
}
case TRANSACTION_vibrator_on:
{
data.enforceInterface(DESCRIPTOR);
int _arg0;
_arg0 = data.readInt();
int _result = this.vibrator_on(_arg0);
reply.writeNoException();
reply.writeInt(_result);
return true;
}
case TRANSACTION_vibrator_off:
{
data.enforceInterface(DESCRIPTOR);
int _result = this.vibrator_off();
reply.writeNoException();
reply.writeInt(_result);
return true;
}
}
return super.onTransact(code, data, reply, flags);
}
private static class Proxy implements angelstone.hardwarebridge.IHardwareBridge
{
private android.os.IBinder mRemote;
Proxy(android.os.IBinder remote)
{
mRemote = remote;
}
public android.os.IBinder asBinder()
{
return mRemote;
}
public java.lang.String getInterfaceDescriptor()
{
return DESCRIPTOR;
}
public int vibrator_on(int timeout) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeInt(timeout);
mRemote.transact(Stub.TRANSACTION_vibrator_on, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
public int vibrator_off() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_vibrator_off, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
}
static final int TRANSACTION_vibrator_on = (IBinder.FIRST_CALL_TRANSACTION + 0);
static final int TRANSACTION_vibrator_off = (IBinder.FIRST_CALL_TRANSACTION + 1);
}
public int vibrator_on(int timeout) throws android.os.RemoteException;
public int vibrator_off() throws android.os.RemoteException;
}
