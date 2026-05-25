# Штрафное задание №2. Отчёт в CI

Напомню, что [по правилам курса](https://github.com/stu-cpp/tictactoe-course#%D1%82%D1%80%D0%B5%D0%B1%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D1%8F-%D0%BA-%D0%BE%D1%82%D1%87%D0%B5%D1%82%D1%83) отчёт следует делать в LaTeX или Typst.

Настройте CI, используя GitHub Actions, который бы на каждый push или Pull Request в основную ветку генерировал PDF-файл отчёта по его исходному LaTeX-коду (ну или typst-коду) и сохранял бы в артефактах.

Можно использовать расширения наподобие [latex-actions](https://github.com/marketplace/actions/github-action-for-latex) или просто всё делать вручную:

```bash
sudo apt-get install -y texlive texlive-lang-cyrillic texlive-fonts-recommended texlive-fonts-extra texlive-latex-extra
pdflatex -interaction=nonstopmode main.tex
pdflatex -interaction=nonstopmode main.tex  # повтор для списка литературы и библиографии
```

Допустимо, если используются какие-то сложные LaTeX-пакеты или надстройки, которые вызывают серьёзные проблемы при компиляции в CI, генерировать только часть отчёта, которая не зависит от этих пакетов.