@echo off

npm install -g hexo-cli

hexo init Hexo

cd Hexo

npm install

git clone https://github.com/ppoffice/hexo-theme-icarus.git themes/icarus

rem http://localhost:4000

npm install hexo-deployer-git --save
npm install hexo-math --save

conda config --add channels conda-forge
conda config --set channel_priority flexible
conda install -c gouarin xeus-cling
conda install python=3.8
jupyter kernelspec list
conda config --set proxy_servers.http http://id:pw@address:port