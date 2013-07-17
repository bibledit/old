<h1>Test</h1>
<form style="border:1px #bbb solid;padding:1em;">
  <textarea name="description" id="id_description" class="rte-zone">Test text</textarea>
</form>
<script src="../rte/jquery.rte.js"></script>
<script type="text/javascript" src="test.js"></script>
<script type="text/javascript">
$(document).ready (function () {
  $('.rte-zone').rte("css url", "toolbox images url");
});
</script>
