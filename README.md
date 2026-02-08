# Launch a HTTP proxy server over SSH

## Usage

```bash
pansy -d -H xxx.xxx.xxx.xxx -P 22 -U whoami -K ~/.ssh/id_ed25519 -p 8000
```

## Testing

```bash
curl -v -x http://127.0.0.1:8000 https://www.google.com
```
