# PANSY - Launch a HTTP proxy server over SSH.

## Building

  ```bash
  git clone https://github.com/saturn-xiv/pansy.git $HOME/workspace/pansy
  cd $HOME/workspace/pansy/
  git submodule update --init --recursive
  
  ./build.sh
  ```

## Usage

  ```bash
  # Generate a sample config
  pansy create-new-user -n "Who am i" -p "change-me"
  # Run a proxy server on debug mode
  pandy -d start-proxy-server -c config.toml -p 8000 -H host -P "change-me"
  # Then append the public key into your ~/.ssh/authorized_keys
  HTTP_PROXY="http://127.0.0.1:8000" HTTPS_PROXY="http://127.0.0.1:8000" curl -v https://www.google.com
  ```

## Documents

- [MacOS Cross-Toolchain for Linux and *BSD](https://github.com/tpoechtrager/osxcross)
