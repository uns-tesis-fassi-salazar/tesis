export class UserNotificationSubscription {
    public uid: string;
    public endpoint: string;
    public expirationTime: number;
    public keys: Record<string,string>;


    constructor(uid?: string, endpoint?: string, expirationTime?: number, keys?) {
        this.uid = uid || '';
        this.endpoint = endpoint || '';
        this.expirationTime = expirationTime || null;
        this.keys = keys || null;
    }
}