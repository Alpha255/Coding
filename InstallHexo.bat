@echo off

npm install -g hexo-cli

hexo init Hexo

cd Hexo

npm install

git clone https://github.com/ppoffice/hexo-theme-icarus.git themes/icarus

rem http://localhost:4000

npm install hexo-deployer-git --save