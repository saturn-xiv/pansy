# PANSY - Launch a HTTP proxy server over SSH.

## Building

  ```bash
  git clone https://github.com/saturn-xiv/pansy.git
  git submodule update --init --recursive
  
  ./build.sh
  ```

## Usage

  ```bash
  # Generate a sample config
  pansy create-new-user -n "Who am i" -p "change-me"
  # Run on debug mode
  pandy -d -c config.toml -H xxx.xxx.xxx.xxx -P 22 -U proxy 
  ```
