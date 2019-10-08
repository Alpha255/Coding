行列式
- 行列式有两行(列)完全相同, 则此行列式等于0
- 行列式中如果有两行(列)元素成比例, 则此行列式等于0
- 把行列式的某一行(列)的各元素乘以同一个数, 然后加到另一行(列)对应的元素, 行列式不变
- 计算行列式常用的一种方法就是利用运算r<sub>i</sub>+kr<sub>j</sub>把行列式化为上三角行列式
- 行列式等于它的任一行(列)的各元素与其对应的代数余子式乘积之和
- 一个n阶行列式, 如果其中第i行所有元素除(i, j)元a<sub>ij</sub>外都为零, 那么这个行列式等于a<sub>ij</sub>与它的代数余子式的乘积

矩阵
- 单位矩阵表示为E
- 矩阵的转置
  - (A<sup>T</sup>)<sup>T</sup> = A
  - (A + B)<sup>T</sup> = A<sup>T</sup> + B<sup>T</sup>
  - ($\lambda$A)<sup>T</sup> = $\lambda$A<sup>T</sup>
  - (AB)<sup>T</sup> = B<sup>T</sup>A<sup>T</sup>
- n阶方阵的行列式
  - | A<sup>T</sup> | = | A |
  - | $\lambda$A | = $\lambda$<sup>n</sup> | A |
  - | AB | = | A | | B |

行列式 | A |的各个元素的代数余子式A<sub>ij</sub>所构成的矩阵称为A的伴随矩阵, 记为A<sup>*</sup>
  - A A<sup>*</sup> = A<sup>*</sup> A = | A | E

逆矩阵
  - A<sup>-1</sup> = ${1\over | A |}$ A<sup>*</sup>

$$
\left[
 \begin{matrix}
   a & b & c & d & e\\
   f & g & h & i & j \\
   k & l & m & n & o \\
   p & q & r & s & t
  \end{matrix} 
\right]
*
\left[
 \begin{matrix}
   a & b & c & d & e\\
   f & g & h & i & j \\
   k & l & m & n & o \\
   p & q & r & s & t
  \end{matrix} 
\right]
$$